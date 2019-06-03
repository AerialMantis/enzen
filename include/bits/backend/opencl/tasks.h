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

template <typename NextSender, typename Function>
class opencl_transform_task;

template <typename Executor, typename Task>
class opencl_via_task;

template <typename Executor, typename Signal>
class opencl_transform_receiver {
 public:
  opencl_transform_receiver(Executor executor, Signal signal)
      : executor_{executor}, signal_{signal} {}

  template <typename Value>
  void value(Value &&value) {
    // TODO: Currently ignoring value
    // TODO: This could be genealized to a promise type object
    executor_.trigger_signal(signal_);
  }

  void done() { enzen::set_done(receiver_); }
  template <typename Error>
  void error(Error &&error) noexcept {
    // TODO: Currently ignoring value, callback withh have to handle results and
    // errors
  }

  Executor get_executor() const noexcept { return executor_; };

 private:
  Executor executor_;
  Signal signal_;
};

template <typename NextSender, typename Function>
class opencl_transform_task;

template <typename T1, typename T2, typename Function>
class opencl_transform_task<opencl_via_task<T1, T2>, Function> {
 public:
  using next_sender_t = typename opencl_via_task<T1, T2>;
  using executor_t = typename next_sender_t::executor_t;

  opencl_transform_task(next_sender_t nextSender, Function function)
      : nextSender_{nextSender}, function_{function} {}

  template <typename IncomingReceiver>
  void submit(IncomingReceiver incomingReceiver) noexcept {
    try {
      auto openclTransformReceiver =
          this->get_executor()
              .template lazy_execute<Function, IncomingReceiver>(
                  function_, std::move(incomingReceiver),
                  enzen::shape{1, 1, 1});

      enzen::submit(std::move(nextSender_), openclTransformReceiver);
    } catch (...) {
      enzen::set_error(incomingReceiver, std::current_exception());
    }
  }

  typename next_sender_t::executor_t get_executor() const noexcept {
    return nextSender_.get_executor();
  };

 private:
  next_sender_t nextSender_;
  Function function_;
};

template <typename T1, typename T2, typename Function>
class opencl_transform_task<opencl_transform_task<T1, T2>, Function> {
 public:
  using next_sender_t = typename opencl_via_task<T1, T2>;
  using executor_t = typename next_sender_t::executor_t;

  opencl_transform_task(next_sender_t nextSender, Function function)
      : nextSender_{nextSender}, function_{function} {}

  template <typename IncomingReceiver>
  void submit(IncomingReceiver incomingReceiver) noexcept {
    try {
      auto openclTransformReceiver =
          this->get_executor()
              .template lazy_execute<Function, IncomingReceiver>(
                  function_, std::move(incomingReceiver),
                  enzen::shape{1, 1, 1});

      enzen::submit(std::move(nextSender_), openclTransformReceiver);
    } catch (...) {
      enzen::set_error(incomingReceiver, std::current_exception());
    }
  }

  typename next_sender_t::executor_t get_executor() const noexcept {
    return nextSender_.get_executor();
  };

 private:
  next_sender_t nextSender_;
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

template <typename Executor, typename Task>
class opencl_via_task {
 public:
  using executor_t = Executor;

  opencl_via_task(Executor executor, Task task)
      : executor_{std::move(executor)}, task_{std::move(task)} {}

  // TODO(Gordon): This function should be r-value qualified
  template <typename IncomingReceiver>
  void submit(IncomingReceiver incomingReceiver) noexcept {
    enzen::submit(std::move(task_), opencl_via_receiver<IncomingReceiver>{
                                        std::move(incomingReceiver)});
  }

  Executor get_executor() const noexcept { return executor_; };

 private:
  Executor executor_;
  Task task_;
};

}  // namespace enzen

#endif  // __ENZEN_BACKEND_OPENCL_TASKS_H__