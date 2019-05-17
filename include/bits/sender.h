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

#ifndef __ENZEN_SENDER_H__
#define __ENZEN_SENDER_H__

namespace enzen {

template <class Receiver>
void set_done(Receiver receiver) {
  receiver.done();
}

template <class Receiver, class Error>
void set_error(Receiver receiver, Error error) {
  receiver.error(error);
}

template <class Receiver, class Value>
void set_value(Receiver receiver, Value value) {
  receiver.value(value);
}

template <class Sender, class Receiver>
void submit(Sender sender, Receiver receiver) {
  sender.submit(receiver);
}

template <typename Value>
class just_task {
 public:
  just_task(Value value) : value_{value} {}

  template <typename Receiver>
  void submit(Receiver receiver) {
    enzen::set_value(receiver, value_);
  }

 private:
  Value value_;
};

template <typename Value>
just_task<Value> just(Value value) {
  return just_task<Value>{std::move(value)};
}

template <typename Executor, typename Task>
auto via(Executor executor, Task task) {
  return typename Executor::backend_t::template via_task_t<Executor, Task>{
      std::move(executor), std::move(task)};
}

template <typename Task, typename Function>
class transform_task {
 public:
  transform_task(Task task, Function function)
      : task_{task}, function_{function} {}

  template <typename Receiver>
  class wrapped_receiver {
   public:
    wrapped_receiver(Function function, Receiver receiver)
        : function_{function}, receiver_{receiver} {}

    template <typename Value>
    void value(Value &&value) {
      enzen::set_value(receiver_, std::invoke(std::move(function_),
                                              static_cast<Value &&>(value)));
    }

    void done() { enzen::set_done(receiver_); }
    template <typename Error>
    void error(Error &&error) noexcept {
      enzen::set_error(receiver_, static_cast<Error &&>(error));
    }

   private:
    Function function_;
    Receiver receiver_;
  };

  template <typename Receiver>
  void submit(Receiver receiver) noexcept {
    try {
      enzen::submit(std::move(task_),
                    wrapped_receiver<Receiver>{std::move(function_),
                                               std::move(receiver)});
    } catch (...) {
      enzen::set_error(receiver, std::current_exception());
    }
  }

 private:
  Task task_;
  Function function_;
};

template <typename Task, typename Function>
auto transform(Task task, Function function) {
  return transform_task<Task, Function>{std::move(task), std::move(function)};
}

}  // namespace enzen

#endif  // __ENZEN_SENDER_H__