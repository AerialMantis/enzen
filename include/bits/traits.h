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

#ifndef __ENZEN_TRAITS_H__
#define __ENZEN_TRAITS_H__

#include <type_traits>

namespace enzen {

/*!
 * @brief Template trait that specifies whether a particular type is an
 * executor. Inherits from std::false_type unless specialised for a valid
 * executor type to inherit from std::true_type.
 * @tparam Entity The type used to derive the value of the trait.
 */
template <typename Entity>
struct is_executor : public std::false_type {};

/*!
 * @brief Constexpr variable template that specifies wheter a particular type is
 * an executor. Value is derived from @ref is_executor.
 * @tparam Entity A type which may or may not be an executor type.
 * @tparam Entity The type used to derive the value of the trait.
 */
template <typename Entity>
constexpr inline bool is_executor_v = is_executor<Entity>::value;

/*!
 * @brief Template trait that specifies whether a particular type is a oneway
 * executor. Inherits from std::false_type unless specialised for a valid oneway
 * executor type to inherit from std::true_type.
 * @tparam Entity The type used to derive the value of the trait.
 */
template <typename Executor>
struct is_oneway_executor : public std::false_type {};

/*!
 * @brief Constexpr variable template that specifies wheter a particular type is
 * a oneway executor. Value is derived from @ref is_oneway_executor.
 * @tparam Entity A type which may or may not be a oneway executor type.
 * @tparam Entity The type used to derive the value of the trait.
 */
template <typename Executor>
constexpr inline bool is_oneway_executor_v =
    is_oneway_executor<Executor>::value;

/*!
 * @brief Template trait that specifies whether a particular type is a bulk
 * oneway executor. Inherits from std::false_type unless specialised for a valid
 * bulk oneway executor type to inherit from std::true_type.
 * @tparam Entity The type used to derive the value of the trait.
 */
template <typename Executor>
struct is_bulk_oneway_executor : public std::false_type {};

/*!
 * @brief Constexpr variable template that specifies wheter a particular type is
 * a bulk oneway executor. Value is derived from @ref is_bulk_oneway_executor.
 * @tparam Entity A type which may or may not be a bulk oneway executor type.
 * @tparam Entity The type used to derive the value of the trait.
 */
template <typename Executor>
constexpr inline bool is_bulk_oneway_executor_v =
    is_bulk_oneway_executor<Executor>::value;

/*!
 * @brief Template trait that specifies whether a particular type is a twoway
 * executor. Inherits from std::false_type unless specialised for a valid twoway
 * executor type to inherit from std::true_type.
 * @tparam Entity The type used to derive the value of the trait.
 */
template <typename Executor>
struct is_twoway_executor : public std::false_type {};

/*!
 * @brief Constexpr variable template that specifies wheter a particular type is
 * a twoway executor. Value is derived from @ref is_twoway_executor.
 * @tparam Entity A type which may or may not be a twoway executor type.
 * @tparam Entity The type used to derive the value of the trait.
 */
template <typename Executor>
constexpr inline bool is_twoway_executor_v =
    is_twoway_executor<Executor>::value;

/*!
 * @brief Template trait that specifies whether a particular type is a bulk
 * twoway executor. Inherits from std::false_type unless specialised for a valid
 * bulk twoway executor type to inherit from std::true_type.
 * @tparam Entity The type used to derive the value of the trait.
 */
template <typename Executor>
struct is_bulk_twoway_executor : public std::false_type {};

/*!
 * @brief Constexpr variable template that specifies wheter a particular type is
 * a bulk twoway executor. Value is derived from @ref is_bulk_twoway_executor.
 * @tparam Entity A type which may or may not be a bulk twoway executor type.
 * @tparam Entity The type used to derive the value of the trait.
 */
template <typename Executor>
constexpr inline bool is_bulk_twoway_executor_v =
    is_bulk_twoway_executor<Executor>::value;

/*!
 * @brief Template trait that specifies whether a particular type is a lazy
 * executor. Inherits from std::false_type unless specialised for a valid lazy
 * executor type to inherit from std::true_type.
 * @tparam Entity The type used to derive the value of the trait.
 */
template <typename Executor>
struct is_lazy_executor : public std::false_type {};

/*!
 * @brief Constexpr variable template that specifies wheter a particular type is
 * a lazy executor. Value is derived from @ref is_lazy_executor.
 * @tparam Entity A type which may or may not be an lazy executor type.
 * @tparam Entity The type used to derive the value of the trait.
 */
template <typename Executor>
constexpr inline bool is_lazy_executor_v = is_lazy_executor<Executor>::value;

}  // namespace enzen

#endif  // __ENZEN_TRAITS_H__
