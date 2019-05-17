/*
Copyright 2018 Gordon Brown

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

#ifndef __ENZEN_COMMON_DEBUG_H__
#define __ENZEN_COMMON_DEBUG_H__

#include <mutex>

#ifdef ENZEN_VERBOSE
#define ENZEN_DEBUG_LOG(...) detail::debug_logger::atomic_print(__VA_ARGS__);
#else
#define ENZEN_DEBUG_LOG(...)
#endif  // ENZEN_VERBOSE

#ifdef ENZEN_VERBOSE
#define ENZEN_LINE_BREAK()                                                     \
  detail::debug_logger::atomic_print(                                          \
      "//////////////////////////////////////////////////////////////////////" \
      "//////////");
#else
#define ENZEN_LINE_BREAK()
#endif  // ENZEN_VERBOSE

std::string indent(int length) {
  std::string str{};
  for (int i = 0; i < length; ++i) {
    str += " ";
  }
  return str;
}

namespace enzen::detail {

class debug_logger {
 public:
  template <typename... Args>
  static void atomic_print(Args... args) {
    static std::mutex mutex_;

    auto lock = std::lock_guard<std::mutex>{mutex_};
    (std::cout << ... << args) << std::endl;
  }
};

void print_debug_log(std::string msg) {}

}  // namespace enzen::detail

#endif  // __ENZEN_COMMON_DEBUG_H__