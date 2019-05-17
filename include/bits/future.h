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

#ifndef __ENZEN_FUTURE_H__
#define __ENZEN_FUTURE_H__

#include <memory>
#include <mutex>

namespace enzen {

template <typename ValueType>
class promise;

template <typename ValueType>
class future_shared_state {
  using value_t = ValueType;
  using exception_t = std::exception_ptr;
  using mutex_t = std::mutex;

 public:
  future_shared_state()
      : value_{static_cast<ValueType>(0)},
        isValueSet_{false},
        isExceptionSet_{false} {}

  void set_value(value_t value) noexcept {
    std::lock_guard<mutex_t> lockGuard(mutex_);
    isValueSet_ = true;
    value_ = value;
  }

  void set_exception(exception_t exception) {
    std::lock_guard<mutex_t> lockGuard(mutex_);
    isExceptionSet_ = true;
    exception_ = exception;
  }

  value_t get_value() const noexcept {
    std::lock_guard<mutex_t> lockGuard(mutex_);
    if (!isValueSet_) {
      throw std::exception(
          "Attempted to access future shared state value "
          "that is not available.");
    }
    return value_;
  }

  exception_t get_exception() const noexcept {
    if (!isExceptionSet_) {
      throw std::exception(
          "Attempted to access future shared state "
          "exception that is not available.");
    }
    return exception_;
  }

  bool is_value_or_exception_set() const noexcept {
    std::lock_guard<mutex_t> lockGuard(mutex_);
    return isValueSet_ || isExceptionSet_;
  }

  bool is_value_set() const noexcept {
    std::lock_guard<mutex_t> lockGuard(mutex_);
    return isValueSet_;
  }

  bool is_exception_set() const noexcept {
    std::lock_guard<mutex_t> lockGuard(mutex_);
    return isExceptionSet_;
  }

 private:
  mutable mutex_t mutex_;
  value_t value_;
  exception_t exception_;
  bool isValueSet_;
  bool isExceptionSet_;
};

template <typename ValueType>
class future {
  using value_t = ValueType;
  using exception_t = std::exception_ptr;
  using shared_state_ptr_t =
      typename std::shared_ptr<future_shared_state<value_t>>;

  friend class promise<ValueType>;

 public:
  bool valid() const noexcept {
    return sharedSatate_->is_value_or_exception_set();
  }

  void wait() const {
    while (!sharedState_->is_value_or_exception_set()) {
    }
  }

  value_t get() {
    while (!sharedState_->is_value_or_exception_set()) {
    }
    if (sharedState_->is_exception_set()) {
      std::rethrow_exception(sharedState_->get_exception());
    } else {
      return sharedState_->get_value();
    }
  }

  // TODO(Gordon): Implement then
#if 0
  template <typename Continuation>
  future<ValueType> then(Continuation&& continuation) {}
#endif

// TODO(Gordon): Implement wait_for
#if 0
  void wait_for() {}
#endif

  // TODO(Gordon): Implement wait_until
#if 0
  void wait_until() {}
#endif

 private:
  future(shared_state_ptr_t sharedStatePtr) noexcept
      : sharedState_{sharedStatePtr} {}

  shared_state_ptr_t sharedState_;
};

template <typename ValueType>
class promise {
  using value_t = ValueType;
  using exception_t = std::exception_ptr;
  using shared_state_ptr_t =
      typename std::shared_ptr<future_shared_state<value_t>>;

 public:
  promise() noexcept
      : sharedStatePtr_{std::make_shared<future_shared_state<ValueType>>()} {}

  future<value_t> get_future() { return future<ValueType>{sharedStatePtr_}; }

  void set_value(value_t value) { sharedStatePtr_->set_value(value); }

  void set_exception(exception_t exception) {
    sharedStatePtr_->set_exception(exception);
  }

 private:
  shared_state_ptr_t sharedStatePtr_;
};

}  // namespace enzen

#endif  // __ENZEN_FUTURE_H__
