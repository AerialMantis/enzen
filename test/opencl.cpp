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

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#define ENZEN_OPENCL_BACKEND

#include <execution>

// TODO(Gordon): Currently all tests are using member functions, rather than
// CPOs due to current C++ support in compute++

using namespace std::chrono_literals;

template <int index>
class kernel {};

template <typename SubExecutor>
class via_receiver {
 public:
  via_receiver(){};

  void value(enzen::opencl_sub_executor subExec) {}

 private:
};

template <typename ValueType>
class value_receiver {
 public:
  value_receiver(enzen::device_ptr<ValueType> valuePtr)
      : valuePtr_{valuePtr} {};

  void value(ValueType value) { *valuePtr_ = value; }

  template <typename Error>
  void error(Error &&error) noexcept {}

  void done() {}

 private:
  enzen::device_ptr<ValueType> valuePtr_;
};

// OpenCL Context Tests

TEST_CASE("construct_context", "opencl") {
  auto openclContext = enzen::opencl_context{};

  REQUIRE(true);
}

TEST_CASE("retrieve_executor", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  REQUIRE(enzen::is_executor_v<decltype(exec)>);
}

TEST_CASE("require_concept_oneway", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto onewayExec = exec.require_concept(enzen::oneway);

  REQUIRE(enzen::is_executor_v<decltype(onewayExec)> == true);
  REQUIRE(enzen::is_oneway_executor_v<decltype(onewayExec)> == true);
  REQUIRE(enzen::is_bulk_oneway_executor_v<decltype(onewayExec)> == false);
  REQUIRE(enzen::is_twoway_executor_v<decltype(onewayExec)> == false);
  REQUIRE(enzen::is_bulk_twoway_executor_v<decltype(onewayExec)> == false);
  REQUIRE(enzen::is_lazy_executor_v<decltype(onewayExec)> == false);
}

TEST_CASE("require_concept_bulk_oneway", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto bulkOnewayExec = exec.require_concept(enzen::bulk_oneway);

  REQUIRE(enzen::is_executor_v<decltype(bulkOnewayExec)> == true);
  REQUIRE(enzen::is_oneway_executor_v<decltype(bulkOnewayExec)> == false);
  REQUIRE(enzen::is_bulk_oneway_executor_v<decltype(bulkOnewayExec)> == true);
  REQUIRE(enzen::is_twoway_executor_v<decltype(bulkOnewayExec)> == false);
  REQUIRE(enzen::is_bulk_twoway_executor_v<decltype(bulkOnewayExec)> == false);
  REQUIRE(enzen::is_lazy_executor_v<decltype(bulkOnewayExec)> == false);
}

TEST_CASE("require_concept_twoway", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto twowayExec = exec.require_concept(enzen::twoway);

  REQUIRE(enzen::is_executor_v<decltype(twowayExec)> == true);
  REQUIRE(enzen::is_oneway_executor_v<decltype(twowayExec)> == false);
  REQUIRE(enzen::is_bulk_oneway_executor_v<decltype(twowayExec)> == false);
  REQUIRE(enzen::is_twoway_executor_v<decltype(twowayExec)> == true);
  REQUIRE(enzen::is_bulk_twoway_executor_v<decltype(twowayExec)> == false);
  REQUIRE(enzen::is_lazy_executor_v<decltype(twowayExec)> == false);
}

TEST_CASE("require_concept_bulk_twoway", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto bulkTwowayExec = exec.require_concept(enzen::bulk_twoway);

  REQUIRE(enzen::is_executor_v<decltype(bulkTwowayExec)> == true);
  REQUIRE(enzen::is_oneway_executor_v<decltype(bulkTwowayExec)> == false);
  REQUIRE(enzen::is_bulk_oneway_executor_v<decltype(bulkTwowayExec)> == false);
  REQUIRE(enzen::is_twoway_executor_v<decltype(bulkTwowayExec)> == false);
  REQUIRE(enzen::is_bulk_twoway_executor_v<decltype(bulkTwowayExec)> == true);
  REQUIRE(enzen::is_lazy_executor_v<decltype(bulkTwowayExec)> == false);
}

TEST_CASE("require_concept_lazy", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto lazyExec = exec.require_concept(enzen::lazy);

  REQUIRE(enzen::is_executor_v<decltype(lazyExec)> == true);
  REQUIRE(enzen::is_oneway_executor_v<decltype(lazyExec)> == false);
  REQUIRE(enzen::is_bulk_oneway_executor_v<decltype(lazyExec)> == false);
  REQUIRE(enzen::is_twoway_executor_v<decltype(lazyExec)> == false);
  REQUIRE(enzen::is_bulk_twoway_executor_v<decltype(lazyExec)> == false);
  REQUIRE(enzen::is_lazy_executor_v<decltype(lazyExec)> == true);
}

TEST_CASE("require_blocking_never", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto neverBlockingExec = exec.require(enzen::blocking.never);

  REQUIRE(neverBlockingExec.query(enzen::blocking) == enzen::blocking.never);
  REQUIRE(neverBlockingExec.query(enzen::blocking) != enzen::blocking.possibly);
  REQUIRE(neverBlockingExec.query(enzen::blocking) != enzen::blocking.always);
}

TEST_CASE("require_blocking_possibly", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto possiblyBlockingExec = exec.require(enzen::blocking.possibly);

  REQUIRE(possiblyBlockingExec.query(enzen::blocking) != enzen::blocking.never);
  REQUIRE(possiblyBlockingExec.query(enzen::blocking) ==
          enzen::blocking.possibly);
  REQUIRE(possiblyBlockingExec.query(enzen::blocking) !=
          enzen::blocking.always);
}

TEST_CASE("require_blocking_always", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto alwaysBlockingExec = exec.require(enzen::blocking.always);

  REQUIRE(alwaysBlockingExec.query(enzen::blocking) != enzen::blocking.never);
  REQUIRE(alwaysBlockingExec.query(enzen::blocking) !=
          enzen::blocking.possibly);
  REQUIRE(alwaysBlockingExec.query(enzen::blocking) == enzen::blocking.always);
}

TEST_CASE("require_name", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto namedExec = exec.require(enzen::name<kernel<0>>);

  REQUIRE(true);
}

TEST_CASE("oneway_execute", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto res = int{-1};
  auto resPtr = enzen::device_ptr<int>{&res};

  exec.require(enzen::name<kernel<1>>).execute([resPtr]() { *resPtr = 1234; });

  REQUIRE(res == 1234);
}

TEST_CASE("always_blocking_oneway_execute", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto alwaysBlockingExec = exec.require(enzen::blocking.always);

  auto res = int{-1};
  auto resPtr = enzen::device_ptr<int>{&res};

  alwaysBlockingExec.require(enzen::name<kernel<2>>).execute([resPtr]() {
    *resPtr = 1234;
  });

  REQUIRE(res == 1234);
}

TEST_CASE("never_blocking_oneway_execute", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto neverBlockingExec = exec.require(enzen::blocking.never);

  auto res = int{-1};
  auto resPtr = enzen::device_ptr<int>{&res};

  neverBlockingExec.require(enzen::name<kernel<3>>).execute([resPtr]() {
    *resPtr = 1234;
  });

  openclContext.wait();

  REQUIRE(res == 1234);
}

TEST_CASE("bulk_oneway_execute", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto bulkOnewayExec = exec.require_concept(enzen::bulk_oneway);

  int res[32] = {-1};
  auto resPtr = enzen::device_ptr<int>{res, 32};

  bulkOnewayExec.require(enzen::name<kernel<4>>)
      .bulk_execute(
          [resPtr](enzen ::index idx) {
            resPtr[idx[0]] = static_cast<int>(idx[0]);
          },
          enzen::shape{32, 1, 1});

  for (int i = 0; i < 32; ++i) {
    REQUIRE(res[i] == i);
  }
}

TEST_CASE("always_blocking_bulk_oneway_execute", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto bulkOnewayExec = exec.require_concept(enzen::bulk_oneway);

  auto alwaysBlockingBulkOnewayExec =
      bulkOnewayExec.require(enzen::blocking.always);

  int res[32] = {-1};
  auto resPtr = enzen::device_ptr<int>{res, 32};

  alwaysBlockingBulkOnewayExec.require(enzen::name<kernel<5>>)
      .bulk_execute(
          [resPtr](enzen ::index idx) {
            resPtr[idx[0]] = static_cast<int>(idx[0]);
          },
          enzen::shape{32, 1, 1});

  for (int i = 0; i < 32; ++i) {
    REQUIRE(res[i] == i);
  }
}

TEST_CASE("never_blocking_bulk_oneway_execute", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto bulkOnewayExec = exec.require_concept(enzen::bulk_oneway);

  auto neverBlockingBulkOnewayExec =
      bulkOnewayExec.require(enzen::blocking.never);

  int res[32] = {-1};
  auto resPtr = enzen::device_ptr<int>{res, 32};

  neverBlockingBulkOnewayExec.require(enzen::name<kernel<6>>)
      .bulk_execute(
          [resPtr](enzen ::index idx) {
            resPtr[idx[0]] = static_cast<int>(idx[0]);
          },
          enzen::shape{32, 1, 1});

  openclContext.wait();

  for (int i = 0; i < 32; ++i) {
    REQUIRE(res[i] == i);
  }
}

// TODO(Gordon): Implement eager twoway for OpenCL context
#if 0
TEST_CASE("twoway_execute", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto twowayExecutor = exec.require_concept(enzen::twoway);

  auto fut = twowayExecutor.twoway_execute([]() { return 1234; });

  REQUIRE(fut.get() == 1234);
}
#endif

TEST_CASE("schedule", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto lazyExec =
      exec.require_concept(enzen::lazy).require(enzen::name<kernel<8>>);

  auto s1 = lazyExec.schedule();

  enzen::submit(s1, via_receiver<decltype(lazyExec)>{});
}

// TODO(Gordon): Currently disabld due to bug in OpenCL back-end
#if 0
TEST_CASE("via_and_transform", "opencl") {
  auto openclContext = enzen::opencl_context{};

  auto exec = openclContext.executor();

  auto lazyExec =
      exec.require_concept(enzen::lazy).require(enzen::name<kernel<8>>);

  auto s1 = enzen::just(21);

  auto s2 = enzen::via(lazyExec, s1);

  auto s3 = enzen::transform(s2, [=](int value) { return value * 2; });

  auto res = int{-1};
  auto resPtr = enzen::device_ptr<int>{&res};

  submit(s3, value_receiver{resPtr});

  REQUIRE(res == 42);
}
#endif
