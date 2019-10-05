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
  using value_t = Value;

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
auto transform(Task task, Function function) {
  return
      typename Task::executor_t::backend_t::template transform_task_t<Task,
                                                                      Function>{
          std::move(task), function};
}

template <typename Task>
void sync_wait(Task task) {
  auto promise = enzen::promise<typename Task::vaue_t>{};
  auto future = promise.get_future();

  submit(task, promise);

  return future.wait();
}

template <typename Task>
typename Task::value_t sync_get(Task task) {
  auto promise = enzen::promise<typename Task::value_t>{};
  auto future = promise.get_future();

  submit(task, promise);

  return future.get();
}

}  // namespace enzen

#endif  // __ENZEN_SENDER_H__