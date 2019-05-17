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

#ifndef __ENZEN_BACKEND_OPENCL_EXECUTION_CONTEXT_H__
#define __ENZEN_BACKEND_OPENCL_EXECUTION_CONTEXT_H__

#include <bits/debug.h>

namespace enzen {

class opencl_context {
 public:
  using executor_type = opencl_executor;

  opencl_context() : impl_{std::make_shared<detail::opencl_backend>()} {
    impl_->start();
  }

  opencl_context(const opencl_context &) = delete;
  opencl_context(opencl_context &&) = default;
  opencl_context &operator=(const opencl_context &) = delete;
  opencl_context &operator=(opencl_context &&) = default;

  // TODO(Gordon): Implement properties for specifying on0destruction behaviour
  // for execution context
  ~opencl_context() {
    impl_->stop();
    impl_->wait();
    impl_->join();
  }

  void wait() { impl_->wait(); }

  executor_type executor() noexcept {
    return executor_type{impl_, detail::executor_blocking::possibly};
  }

 private:
  std::shared_ptr<detail::opencl_backend> impl_;
};

}  // namespace enzen

#endif  // __ENZEN_BACKEND_OPENCL_EXECUTION_CONTEXT_H__
