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

#ifndef __ENZEN_STATIC_THREAD_POOL_EXECUTOR_H__
#define __ENZEN_STATIC_THREAD_POOL_EXECUTOR_H__

namespace enzen {

namespace detail {

class thread_pool_backend;
}

using static_thread_pool_executor =
    basic_executor<detail::thread_pool_backend,
                   detail::executor_interface::oneway, void>;

template <>
struct is_executor<static_thread_pool_executor> : public std::true_type {};

}  // namespace enzen

#endif  // __ENZEN_STATIC_THREAD_POOL_EXECUTOR_H__