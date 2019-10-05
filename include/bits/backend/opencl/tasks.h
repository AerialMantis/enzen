/*
Copyright 2018 - 2019 Gordon Brown

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef __ENZEN_BACKEND_OPENCL_TASKS_H__
#define __ENZEN_BACKEND_OPENCL_TASKS_H__

// TODO: Would like to avoid including SYCL here
#include <CL/sycl.hpp>

namespace enzen {

template <typename NextTask, typename Function>
class opencl_transform_task;

template <typename Executor, typename Task>
class opencl_via_task;

template <typename Executor, typename Value>
class opencl_transform_receiver {
  using value_t = Value;

 public:
  opencl_transform_receiver(Executor executor, Value *inputPtr, cl_event signal)
      : executor_{executor}, inputPtr_{std::move(inputPtr)}, signal_{signal} {}

  template <typename Value>
  void value(Value &&value) {
    // Set incoming value.
    *inputPtr_ = value;

    // Trigger the OpenCL commands to execute.
    detail::cl_set_user_event_status(signal_, CL_COMPLETE);
  }

  void done() {
    // Cancel the OpenCL commands with the cancellation code.
    detail::cl_set_user_event_status(signal_, -9999);
  }

  template <typename Error>
  void error(Error &&error) noexcept {
    // Cancel the OpenCL commands with the incoming error code.
    detail::cl_set_user_event_status(signal_, error);
  }

  Executor get_executor() const noexcept { return executor_; };

 private:
  Executor executor_;
  Value *inputPtr_;
  cl_event signal_;
};

template <typename NextTask, typename Function>
class opencl_transform_task {
 public:
  using executor_t = typename NextTask::executor_t;
  using next_task_t = NextTask;
  using value_t = std::invoke_result_t<Function, typename NextTask::value_t>;
  using param_t = typename NextTask::value_t;

  opencl_transform_task(next_task_t nextTask, Function function)
      : nextTask_{nextTask}, function_{function} {}

  template <typename IncomingReceiver>
  void submit(IncomingReceiver &incomingReceiver) noexcept {
    try {
      auto openclTransformReceiver =
          this->get_executor()
              .template lazy_execute<param_t, Function, IncomingReceiver>(
                  function_, incomingReceiver, enzen::shape{1, 1, 1});

      enzen::submit(std::move(nextTask_), openclTransformReceiver);
    } catch (...) {
      enzen::set_error(incomingReceiver, std::current_exception());
    }
  }

  typename next_task_t::executor_t get_executor() const noexcept {
    return nextTask_.get_executor();
  };

 private:
  next_task_t nextTask_;
  Function function_;
};

template <typename NextReceiver>
class opencl_via_receiver {
 public:
  opencl_via_receiver(NextReceiver nextReceiver)
      : nextReceiver_{nextReceiver} {}

  template <typename Value>
  void value(Value value) {
    // TODO: Not really sure where schedule() fits in here.
    enzen::set_value(nextReceiver_, value);
  }

  void done() { enzen::set_done(nextReceiver); }

  template <typename Error>
  void error(Error error) noexcept {
    enzen::set_value(nextReceiver_, error);
  }

 private:
  NextReceiver nextReceiver_;
};

template <typename Executor, typename NextTask>
class opencl_via_task {
 public:
  using executor_t = Executor;
  using next_task_t = NextTask;
  using value_t = typename NextTask::value_t;
  using param_t = typename NextTask::value_t;

  opencl_via_task(Executor executor, NextTask nextTask)
      : executor_{std::move(executor)}, nextTask_{std::move(nextTask)} {}

  // TODO(Gordon): This function should be r-value qualified
  template <typename IncomingReceiver>
  void submit(IncomingReceiver incomingReceiver) noexcept {
    enzen::submit(std::move(nextTask_), opencl_via_receiver<IncomingReceiver>{
                                            std::move(incomingReceiver)});
  }

  Executor get_executor() const noexcept { return executor_; };

 private:
  Executor executor_;
  NextTask nextTask_;
};

}  // namespace enzen

#endif  // __ENZEN_BACKEND_OPENCL_TASKS_H__