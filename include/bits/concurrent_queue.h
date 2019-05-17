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

#ifndef __CONCURRENT_QUEUE_H__
#define __CONCURRENT_QUEUE_H__

#include <atomic>
#include <condition_variable>
#include <queue>

namespace enzen {

template <typename ValueType>
class concurrent_queue {
 public:
  concurrent_queue() {}

  void push(ValueType newValue) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(newValue);
  }

  bool try_pop(ValueType &returnValue) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) { return false; }
    returnValue = queue_.front();
    queue_.pop();
    return true; 
  }

  bool empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
  }

 private:
  std::queue<ValueType> queue_;
  mutable std::mutex mutex_;
};

}  // namespace enzen

#endif  // __CONCURRENT_QUEUE_H__