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

#ifndef __ENZEN_STATIC_THREAD_POOL_TASKS_H__
#define __ENZEN_STATIC_THREAD_POOL_TASKS_H__

namespace enzen {

template <typename Executor, typename Task>
class thread_via_task {
  template <typename Value, typename Receiver>
  class value_receiver {
   public:
    value_receiver(Value value, Receiver receiver)
        : value_{value}, receiver_{receiver} {}

    template <typename SubExecutor>
    void value([[maybe_unused]] SubExecutor subExecutor) {
      enzen::set_value(receiver_, std::forward<Value>(value_));
    }

    template <typename SchedulingError>
    void error(SchedulingError &&error) noexcept {
      enzen::set_error(receiver_, std::forward<SchedulingError>(error));
    }

    void done() { enzen::set_done(receiver_); }

   private:
    Value value_;
    Receiver receiver_;
  };

  template <typename Error, typename Receiver>
  class error_receiver {
   public:
    error_receiver(Error error, Receiver receiver)
        : error_{error}, receiver_{receiver} {}

    template <typename SubExecutor>
    void value([[maybe_unused]] SubExecutor subExecutor) {
      enzen::set_error(receiver_, std::forward<ErrorType>(error));
    }

    template <typename SchedulingError>
    void error(SchedulingError &&error) noexcept {
      enzen::set_error(receiver_, std::forward<SchedulingError>(error));
    }

    void done() { enzen::set_done(receiver_); }

   private:
    Error error_;
    Receiver receiver_;
  };

  template <typename Receiver>
  class done_receiver {
   public:
    done_receiver(Receiver receiver) : receiver_{receiver} {}

    template <typename SubExecutor>
    void value([[maybe_unused]] SubExecutor subExecutor) {}

    template <typename SchedulingError>
    void error(SchedulingError &&error) noexcept {
      enzen::set_error(receiver_, std::forward<SchedulingError>(error));
    }

    void done() { enzen::set_done(receiver_); }

   private:
    Receiver receiver_;
  };

  template <typename Receiver>
  class task_receiever {
   public:
    task_receiever(Executor executor, Receiver receiver)
        : executor_{executor}, receiver_{receiver}, valueCalled_{false} {}

    template <typename Value>
    void value(Value value) {
      enzen::submit(executor_.schedule(),
                    value_receiver<Value, Receiver>{std::move(value),
                                                    std::move(receiver_)});
      valueCalled_ = true;
    }

    void done() {
      if (!valueCalled_) {
        enzen::submit(executor_.schedule(),
                      done_receiver<R>{std::move(receiver_)});
      }
    }

    template <typename Error>
    void error(Error error) noexcept {
      try {
        enzen::submit(executor_.schedule(),
                      error_receiver<Error, Receiver>{std::move(error),
                                                      std::move(receiver_)});
      } catch (...) {
        enzen::set_error(receiver_, std::current_exception());
      }
    }

   private:
    Receiver receiver_;
    Executor executor_;
    bool valueCalled_;
  };

 public:
  thread_via_task(Executor executor, Task task)
      : executor_{std::move(executor)}, task_{std::move(task)} {}

  // TODO(Gordon): This function should be r-value qualified
  template <typename Receiver>
  void submit(Receiver receiver) noexcept {
    try {
      enzen::submit(
          std::move(task_),
          task_receiever<Receiver>{std::move(executor_), std::move(receiver)});
    } catch (...) {
      enzen::set_error(receiver, std::current_exception());
    }
  }

 private:
  Executor executor_;
  Task task_;
};

}  // namespace enzen

#endif  // __ENZEN_STATIC_THREAD_POOL_TASKS_H__