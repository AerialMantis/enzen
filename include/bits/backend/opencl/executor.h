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

#ifndef __ENZEN_BACKEND_OPENCL_EXECUTOR_H__
#define __ENZEN_BACKEND_OPENCL_EXECUTOR_H__

namespace enzen {

namespace detail {

class opencl_backend;
}

using opencl_executor =
    basic_executor<detail::opencl_backend, detail::executor_interface::oneway,
                   void>;

template <>
struct is_executor<opencl_executor> : public std::true_type {};

}  // namespace enzen

#endif  // __ENZEN_OPENCL_EXECUTOR_H__
