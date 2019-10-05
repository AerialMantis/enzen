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

#ifndef __ENZEN_BASIC_EXECUTOR_H__
#define __ENZEN_BASIC_EXECUTOR_H__

#include <iterator>

#include <bits/debug.h>

namespace enzen {

namespace detail {

enum class executor_interface {
  oneway,
  bulk_oneway,
  twoway,
  bulk_twoway,
  lazy
};

enum class executor_blocking { always, never, possibly };

}  // namespace detail

template <typename Backend,
          detail::executor_interface Interface =
              detail::executor_interface::oneway,
          typename KernelName = void>
class basic_executor {
 public:
  using backend_t = Backend;
  using sub_executor_t = typename Backend::sub_executor_t;
  using kernel_name_t = KernelName;

  struct schedule_task {
    schedule_task(basic_executor<Backend, Interface, KernelName> taskExec)
        : taskExec_{taskExec} {}

    template <typename Receiver>
    void submit(Receiver &&receiver) {
      taskExec_.impl_->template execute<KernelName>(
          [receiver = std::forward<Receiver &&>(receiver),
           subExec = taskExec_.get_sub_executor()]() {
            set_value(receiver, subExec);
          },
          taskExec_.get_blocking_semantics());
    }

    basic_executor<Backend, Interface, KernelName> taskExec_;
  };

  basic_executor() = default;
  basic_executor(const basic_executor &) = default;
  basic_executor(basic_executor &&) = default;
  basic_executor &operator=(const basic_executor &) = default;
  basic_executor &operator=(basic_executor &&) = default;

  inline friend bool operator==(const basic_executor &lhs,
                                const basic_executor &rhs) {
    return true;
  }

  // TODO (Gordon): This constructor should be private, needs to be fixed.
  basic_executor(std::shared_ptr<Backend> impl,
                 detail::executor_blocking blockingSemantics)
      : impl_{impl}, blockingSemantics_{blockingSemantics} {}

 public:
  virtual ~basic_executor() = default;

  auto require_concept(oneway_t) const noexcept {
    return basic_executor<Backend, detail::executor_interface::oneway,
                          KernelName>{impl_, blockingSemantics_};
  }

  auto require_concept(twoway_t) const noexcept {
    return basic_executor<Backend, detail::executor_interface::twoway,
                          KernelName>{impl_, blockingSemantics_};
  }

  auto require_concept(bulk_oneway_t) const noexcept {
    return basic_executor<Backend, detail::executor_interface::bulk_oneway,
                          KernelName>{impl_, blockingSemantics_};
  }

  auto require_concept(bulk_twoway_t) const noexcept {
    return basic_executor<Backend, detail::executor_interface::bulk_twoway,
                          KernelName>{impl_, blockingSemantics_};
  }

  auto require_concept(lazy_t) const noexcept {
    return basic_executor<Backend, detail::executor_interface::lazy,
                          KernelName>{impl_, blockingSemantics_};
  }

  auto require(blocking_t::always_t) const noexcept {
    return basic_executor<Backend, Interface, KernelName>{
        impl_, detail::executor_blocking::always};
  }

  auto require(blocking_t::never_t) const noexcept {
    return basic_executor<Backend, Interface, KernelName>{
        impl_, detail::executor_blocking::never};
  }

  auto require(blocking_t::possibly_t) const noexcept {
    return basic_executor<Backend, Interface, KernelName>{
        impl_, detail::executor_blocking::possibly};
  }

  template <typename KernelName>
  auto require(name_t<KernelName>) const noexcept {
    return basic_executor<Backend, Interface, KernelName>{impl_,
                                                          blockingSemantics_};
  }

  constexpr blocking_t query(blocking_t) const noexcept {
    switch (blockingSemantics_) {
      case detail::executor_blocking::always:
        return blocking_t::always_t{};
      case detail::executor_blocking::never:
        return blocking_t::never_t{};
      case detail::executor_blocking::possibly:
        return blocking_t::possibly_t{};
      default:
        return blocking_t{};
    }
  }

  template <typename Function, typename AlwaysDeduced = KernelName,
            typename = typename std::enable_if_t<
                std::is_same_v<AlwaysDeduced, KernelName> &&
                Interface == detail::executor_interface::oneway>>
  void execute(Function &&func) {
    impl_->template execute<KernelName>(std::forward<Function &&>(func),
                                        blockingSemantics_);
  }

  template <typename Function, typename AlwaysDeduced = KernelName,
            typename = typename std::enable_if_t<
                std::is_same_v<AlwaysDeduced, KernelName> &&
                Interface == detail::executor_interface::bulk_oneway>>
  void bulk_execute(Function &&func, shape shape) {
    impl_->template bulk_execute<KernelName>(std::forward<Function &&>(func),
                                             shape, blockingSemantics_);
  }

  template <typename Function, typename AlwaysDeduced = KernelName,
            typename = typename std::enable_if_t<
                std::is_same_v<AlwaysDeduced, KernelName> &&
                Interface == detail::executor_interface::twoway>>
  auto twoway_execute(Function &&func) {
    return impl_->template twoway_execute<KernelName>(
        std::forward<Function &&>(func), blockingSemantics_);
  }

  template <typename AlwaysDeduced = KernelName,
            typename = typename std::enable_if_t<
                std::is_same_v<AlwaysDeduced, KernelName> &&
                Interface == detail::executor_interface::lazy>>
  auto schedule() {
    return schedule_task{*this};
  }

  template <typename Param, typename Function, typename Callback,
            typename AlwaysDeduced = KernelName,
            typename = typename std::enable_if_t<
                std::is_same_v<AlwaysDeduced, KernelName> &&
                Interface == detail::executor_interface::lazy>>
  auto lazy_execute(Function func, Callback &callback, enzen::shape shape) {
    return impl_->template lazy_execute<Param, KernelName, Function, Callback>(
        func, callback, shape);
  }

  template <typename Signal>
  void trigger_signal(Signal signal) {
    impl_->trigger_signal(signal);
  }

  std::shared_ptr<Backend> get_impl() {
    return impl_;
  }

 private:
  sub_executor_t get_sub_executor() const noexcept {
    return sub_executor_t{impl_, blockingSemantics_};
  }

  detail::executor_blocking get_blocking_semantics() const noexcept {
    return blockingSemantics_;
  }

  std::shared_ptr<Backend> impl_;
  detail::executor_blocking blockingSemantics_;
};

template <typename Backend, detail::executor_interface Interface,
          typename KernelName>
struct is_executor<basic_executor<Backend, Interface, KernelName>>
    : public std::true_type {};

template <typename Backend, typename KernelName>
struct is_oneway_executor<
    basic_executor<Backend, detail::executor_interface::oneway, KernelName>>
    : public std::true_type {};

template <typename Backend, typename KernelName>
struct is_bulk_oneway_executor<basic_executor<
    Backend, detail::executor_interface::bulk_oneway, KernelName>>
    : public std::true_type {};

template <typename Backend, typename KernelName>
struct is_twoway_executor<
    basic_executor<Backend, detail::executor_interface::twoway, KernelName>>
    : public std::true_type {};

template <typename Backend, typename KernelName>
struct is_bulk_twoway_executor<basic_executor<
    Backend, detail::executor_interface::bulk_twoway, KernelName>>
    : public std::true_type {};

template <typename Backend, typename KernelName>
struct is_lazy_executor<
    basic_executor<Backend, detail::executor_interface::lazy, KernelName>>
    : public std::true_type {};

}  // namespace enzen

#endif  // __ENZEN_BASIC_EXECUTOR_H__
