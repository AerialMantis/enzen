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

#ifndef __ENZEN_STATIC_THREAD_POOL_EXECUTION_CONTEXT_H__
#define __ENZEN_STATIC_THREAD_POOL_EXECUTION_CONTEXT_H__

#include <bits/debug.h>

namespace enzen {

class static_thread_pool {
 public:
  using executor_type = static_thread_pool_executor;

  static_thread_pool(std::size_t numThreads)
      : impl_{std::make_shared<detail::thread_pool_backend>(numThreads)} {
    ENZEN_LINE_BREAK()
    impl_->start();
  }

  static_thread_pool(const static_thread_pool &) = delete;
  static_thread_pool(static_thread_pool &&) = default;
  static_thread_pool &operator=(const static_thread_pool &) = delete;
  static_thread_pool &operator=(static_thread_pool &&) = default;

  ~static_thread_pool() {
    impl_->stop();
    impl_->wait();
    impl_->join();
    ENZEN_LINE_BREAK()
  }

  void attach() { impl_->attach(); }

  void stop() { impl_->stop(); }

  void wait() { impl_->wait(); }

  executor_type executor() noexcept {
    return executor_type{impl_, detail::executor_blocking::possibly};
  }

 private:
  std::shared_ptr<detail::thread_pool_backend> impl_;
};

}  // namespace enzen

#endif  // __ENZEN_STATIC_THREAD_POOL_EXECUTION_CONTEXT_H__
