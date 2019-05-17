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

#ifndef __ENZEN_PROPERTIES_H__
#define __ENZEN_PROPERTIES_H__

#include "propria/prefer.hpp"
#include "propria/query.hpp"
#include "propria/require.hpp"
#include "propria/require_concept.hpp"

namespace enzen {

/*
 * @brief Struct which reflects whether a particular executor type can require a
 * particular set of properties.
 * @tparam Executor Executor type.
 * @tparam Properties Set of properties that are being evaluated for the
 * executor type.
 * @note Inherited from Propria (Copyright (c) 2003-2019 Christopher M.
 * Kohlhoff).
 */
template <typename Executor, typename... Properties>
using can_require = propria::can_require<Executor, Properties...>;

/*
 * @brief Variable template which reflects whether a particular executor type
 * can require a particular set of properties.
 * @tparam Executor Executor type.
 * @tparam Properties Set of properties that are being evaluated for the
 * executor type.
 * @note Inherited from Propria (Copyright (c) 2003-2019 Christopher M.
 * Kohlhoff).
 */
template <typename Executor, typename... Properties>
constexpr bool can_require_v = propria::can_require_v<Executor, Properties...>;

/*
 * @brief Struct which reflects whether a particular executor type can prefer a
 * particular set of properties.
 * @tparam Executor Executor type.
 * @tparam Properties Set of properties that are being evaluated for the
 * executor type.
 * @note Inherited from Propria (Copyright (c) 2003-2019 Christopher M.
 * Kohlhoff).
 */
template <typename Executor, typename... Properties>
using can_prefer = propria::can_prefer<Executor, Properties...>;

/*
 * @brief Variable template which reflects whether a particular executor type
 * can prefer a particular set of properties.
 * @tparam Executor Executor type.
 * @tparam Properties Set of properties that are being evaluated for the
 * executor type.
 * @note Inherited from Propria (Copyright (c) 2003-2019 Christopher M.
 * Kohlhoff).
 */
template <typename Executor, typename... Properties>
constexpr bool can_prefer_v = propria::can_prefer_v<Executor, Properties...>;

/*
 * @brief Struct which reflects whether a particular executor type can query a
 * particular property.
 * @tparam Executor Executor type.
 * @tparam Properties The property that is being evaluated for the executor
 * type.
 * @note Inherited from Propria (Copyright (c) 2003-2019 Christopher M.
 * Kohlhoff).
 */
template <typename Executor, typename Property>
using can_query = propria::can_query<Executor, Property>;

/*
 * @brief Variable template which reflects whether a particular executor type
 * can query a particular property.
 * @tparam Executor Executor type.
 * @tparam Properties The property that is being evaluated for the executor
 * type.
 * @note Inherited from Propria (Copyright (c) 2003-2019 Christopher M.
 * Kohlhoff).
 */
template <typename Executor, typename Property>
constexpr bool can_query_v = propria::can_query_v<Executor, Property>;

namespace {

/*
 * @brief Customization point object for requiring an executor that suports an
 * interface changing property.
 * @note Inherited from Propria (Copyright (c) 2003-2019 Christopher M.
 * Kohlhoff).
 */
static constexpr const propria_require_concept_fn::impl& require_concept =
    propria::require_concept;

/*
 * @brief Customization point object for requiring an executor that suports a
 * series of properties.
 * @note Inherited from Propria (Copyright (c) 2003-2019 Christopher M.
 * Kohlhoff).
 */
static constexpr const propria_require_fn::impl& require = propria::require;

/*
 * @brief Customization point object for prefering that an executor that suport
 * a series of properties.
 * @note Inherited from Propria (Copyright (c) 2003-2019 Christopher M.
 * Kohlhoff).
 */
static constexpr const propria_prefer_fn::impl& prefer = propria::prefer;

/*
 * @brief Customization point object for retreiving the value of a property for
 * an executor.
 * @note Inherited from Propria (Copyright (c) 2003-2019 Christopher M.
 * Kohlhoff).
 */
static constexpr const propria_query_fn::impl& query = propria::query;
}  // namespace

namespace detail {
enum class blocking : int { none = -1, always, never, possibly };
}

struct context_t {
  template <class T>
  static constexpr bool is_applicable_property_v = is_executor_v<T>;

  static constexpr bool is_requirable = false;
  static constexpr bool is_preferable = false;

  // TODO(Gordon): Create a basic_execution_context type that can serve as the
  // polymorphic query result type for context_t.
  using polymorphic_query_result_type = void;

  // TODO(Gordon): Implement static_query_v
#if 0
  template <class Executor>
  static constexpr decltype(auto) static_query_v =
  Executor::query(context_t());
#endif
};

constexpr context_t context{};

struct oneway_t {
  template <class T>
  static constexpr bool is_applicable_property_v = is_executor_v<T>;

  static constexpr bool is_requirable_concept = true;
};

constexpr oneway_t oneway{};

struct bulk_oneway_t {
  template <class T>
  static constexpr bool is_applicable_property_v = is_executor_v<T>;

  static constexpr bool is_requirable_concept = true;
};

constexpr bulk_oneway_t bulk_oneway{};

struct twoway_t {
  template <class T>
  static constexpr bool is_applicable_property_v = is_executor_v<T>;

  static constexpr bool is_requirable_concept = true;
};

constexpr twoway_t twoway{};

struct bulk_twoway_t {
  template <class T>
  static constexpr bool is_applicable_property_v = is_executor_v<T>;

  static constexpr bool is_requirable_concept = true;
};

constexpr bulk_twoway_t bulk_twoway{};

struct lazy_t {
  template <class T>
  static constexpr bool is_applicable_property_v = is_executor_v<T>;

  static constexpr bool is_requirable_concept = true;
};

constexpr lazy_t lazy{};

class blocking_t {
 public:
  class always_t {
   public:
    using polymorphic_query_result_type = blocking_t;

    template <class T>
    static constexpr bool is_applicable_property_v = is_executor_v<T>;

    static constexpr bool is_requirable = true;
    static constexpr bool is_preferable = true;

    // TODO(Gordon): Implement static_query_v
#if 0
    template <class Executor>
    static constexpr auto static_query_v =
      std::declval<Executor>().query(always_t());
#endif

    static constexpr blocking_t value() { return blocking_t(always_t()); }
  };

  static constexpr always_t always{};

  class never_t {
   public:
    using polymorphic_query_result_type = blocking_t;

    template <class T>
    static constexpr bool is_applicable_property_v = is_executor_v<T>;

    static constexpr bool is_requirable = true;
    static constexpr bool is_preferable = true;

    // TODO(Gordon): Implement static_query_v
#if 0
    template <class Executor>
    static constexpr auto static_query_v =
      std::declval<Executor>().query(never_t());
#endif

    static constexpr blocking_t value() { return blocking_t(never_t()); }
  };

  static constexpr never_t never{};

  class possibly_t {
   public:
    using polymorphic_query_result_type = blocking_t;

    template <class T>
    static constexpr bool is_applicable_property_v = is_executor_v<T>;

    static constexpr bool is_requirable = true;
    static constexpr bool is_preferable = true;

    // TODO(Gordon): Implement static_query_v
#if 0
    template <class Executor>
    static constexpr auto static_query_v =
        std::declval<Executor>().query(possibly_t());
#endif

    static constexpr blocking_t value() { return blocking_t(possibly_t()); }
  };

  static constexpr possibly_t possibly{};

  using polymorphic_query_result_type = blocking_t;

  template <class T>
  static constexpr bool is_applicable_property_v = is_executor_v<T>;

  static constexpr bool is_requirable = false;
  static constexpr bool is_preferable = false;

  // TODO(Gordon): Implement static_query_v
#if 0
  template <class Executor>
  static constexpr auto static_query_v =
    std::declval<Executor>().query(blocking_t());
#endif

  constexpr blocking_t() : _value{detail::blocking::none} {}

  constexpr blocking_t(const always_t) : _value{detail::blocking::always} {}

  constexpr blocking_t(const never_t) : _value{detail::blocking::never} {}

  constexpr blocking_t(const possibly_t) : _value{detail::blocking::possibly} {}

  friend constexpr bool operator==(const blocking_t& lhs,
                                   const blocking_t& rhs) {
    return lhs._value == rhs._value;
  }

  friend constexpr bool operator!=(const blocking_t& lhs,
                                   const blocking_t& rhs) {
    return !operator==(lhs, rhs);
  }

 private:
  detail::blocking _value;
};

constexpr blocking_t blocking{};

struct blocking_adaptation_t {};

constexpr blocking_adaptation_t blocking_adaptation;

struct relationship_t {};

constexpr relationship_t relationship;

struct outstanding_work_t {};

constexpr outstanding_work_t outstanding_work;

struct bulk_guarantee_t {};

constexpr bulk_guarantee_t bulk_guarantee;

struct mapping_t {};

constexpr mapping_t mapping;

template <typename ProtoAllocator>
struct allocator_t {};

constexpr allocator_t<void> allocator;

template <typename KernelName>
struct name_t {
  template <typename>
  static constexpr bool is_applicable_property_v = true;

  static constexpr bool is_requirable = true;
};
template <typename KernelName>
constexpr name_t<KernelName> name{};

}  // namespace enzen

#endif  // __ENZEN_PROPERTIES_H__
