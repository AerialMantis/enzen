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

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

// #define ENZEN_VERBOSE

#include <chrono>
#include <execution>
#include <thread>

using namespace std::chrono_literals;

template <int index>
class kernel {};

template <typename SubExecutor>
class via_receiver {
 public:
  via_receiver(){};

  void value(enzen::static_thread_pool_executor subExec) {}

 private:
};

template <typename ValueType>
class value_receiver {
 public:
  value_receiver(ValueType *valuePtr)
      : valuePtr_{valuePtr} {};

  void value(ValueType value) { *valuePtr_ = value; }

  template <typename Error>
  void error(Error &&error) noexcept {}

  void done() {}

 private:
  ValueType *valuePtr_;
};

// Static Thread Pool Tests

TEST_CASE("construct_context", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  REQUIRE(true);
}

TEST_CASE("retrieve_executor", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  REQUIRE(enzen::is_executor_v<decltype(exec)>);
}

// Static Thread Pool Executor Require Tests

TEST_CASE("require_concept_oneway", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto onewayExec = enzen::require_concept(exec, enzen::oneway);

  REQUIRE(enzen::is_executor_v<decltype(onewayExec)> == true);
  REQUIRE(enzen::is_oneway_executor_v<decltype(onewayExec)> == true);
  REQUIRE(enzen::is_bulk_oneway_executor_v<decltype(onewayExec)> == false);
  REQUIRE(enzen::is_twoway_executor_v<decltype(onewayExec)> == false);
  REQUIRE(enzen::is_bulk_twoway_executor_v<decltype(onewayExec)> == false);
  REQUIRE(enzen::is_lazy_executor_v<decltype(onewayExec)> == false);
}

TEST_CASE("require_concept_bulk_oneway", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto bulkOnewayExec = enzen::require_concept(exec, enzen::bulk_oneway);

  REQUIRE(enzen::is_executor_v<decltype(bulkOnewayExec)> == true);
  REQUIRE(enzen::is_oneway_executor_v<decltype(bulkOnewayExec)> == false);
  REQUIRE(enzen::is_bulk_oneway_executor_v<decltype(bulkOnewayExec)> == true);
  REQUIRE(enzen::is_twoway_executor_v<decltype(bulkOnewayExec)> == false);
  REQUIRE(enzen::is_bulk_twoway_executor_v<decltype(bulkOnewayExec)> == false);
  REQUIRE(enzen::is_lazy_executor_v<decltype(bulkOnewayExec)> == false);
}

TEST_CASE("require_concept_twoway", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto twowayExec = enzen::require_concept(exec, enzen::twoway);

  REQUIRE(enzen::is_executor_v<decltype(twowayExec)> == true);
  REQUIRE(enzen::is_oneway_executor_v<decltype(twowayExec)> == false);
  REQUIRE(enzen::is_bulk_oneway_executor_v<decltype(twowayExec)> == false);
  REQUIRE(enzen::is_twoway_executor_v<decltype(twowayExec)> == true);
  REQUIRE(enzen::is_bulk_twoway_executor_v<decltype(twowayExec)> == false);
  REQUIRE(enzen::is_lazy_executor_v<decltype(twowayExec)> == false);
}

TEST_CASE("require_concept_bulk_twoway", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto bulkTwowayExec = enzen::require_concept(exec, enzen::bulk_twoway);

  REQUIRE(enzen::is_executor_v<decltype(bulkTwowayExec)> == true);
  REQUIRE(enzen::is_oneway_executor_v<decltype(bulkTwowayExec)> == false);
  REQUIRE(enzen::is_bulk_oneway_executor_v<decltype(bulkTwowayExec)> == false);
  REQUIRE(enzen::is_twoway_executor_v<decltype(bulkTwowayExec)> == false);
  REQUIRE(enzen::is_bulk_twoway_executor_v<decltype(bulkTwowayExec)> == true);
}

TEST_CASE("require_concept_lazy", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto lazyExec = enzen::require_concept(exec, enzen::lazy);

  REQUIRE(enzen::is_executor_v<decltype(lazyExec)> == true);
  REQUIRE(enzen::is_oneway_executor_v<decltype(lazyExec)> == false);
  REQUIRE(enzen::is_bulk_oneway_executor_v<decltype(lazyExec)> == false);
  REQUIRE(enzen::is_twoway_executor_v<decltype(lazyExec)> == false);
  REQUIRE(enzen::is_bulk_twoway_executor_v<decltype(lazyExec)> == false);
  REQUIRE(enzen::is_lazy_executor_v<decltype(lazyExec)> == true);
}

TEST_CASE("require_blocking_never", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto neverBlockingExec = enzen::require(exec, enzen::blocking.never);

  REQUIRE(enzen::query(neverBlockingExec, enzen::blocking) ==
          enzen::blocking.never);
  REQUIRE(enzen::query(neverBlockingExec, enzen::blocking) !=
          enzen::blocking.possibly);
  REQUIRE(enzen::query(neverBlockingExec, enzen::blocking) !=
          enzen::blocking.always);
}

TEST_CASE("require_blocking_possibly", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto possiblyBlockingExec = enzen::require(exec, enzen::blocking.possibly);

  REQUIRE(enzen::query(possiblyBlockingExec, enzen::blocking) !=
          enzen::blocking.never);
  REQUIRE(enzen::query(possiblyBlockingExec, enzen::blocking) ==
          enzen::blocking.possibly);
  REQUIRE(enzen::query(possiblyBlockingExec, enzen::blocking) !=
          enzen::blocking.always);
}

TEST_CASE("require_blocking_always", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto alwaysBlockingExec = enzen::require(exec, enzen::blocking.always);

  REQUIRE(enzen::query(alwaysBlockingExec, enzen::blocking) !=
          enzen::blocking.never);
  REQUIRE(enzen::query(alwaysBlockingExec, enzen::blocking) !=
          enzen::blocking.possibly);
  REQUIRE(enzen::query(alwaysBlockingExec, enzen::blocking) ==
          enzen::blocking.always);
}

TEST_CASE("require_name", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto namedExec = enzen::require(exec, enzen::name<kernel<0>>);

  REQUIRE(true);
}

TEST_CASE("oneway_execute", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  int res = -1;

  exec.execute([&res]() { res = 1234; });

  REQUIRE(res == 1234);
}

TEST_CASE("always_blocking_oneway_execute", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto alwaysBlockingExec = enzen::require(exec, enzen::blocking.always);

  int res = -1;

  alwaysBlockingExec.execute([&res]() {
    res = 1234;
    std::this_thread::sleep_for(2s);
  });

  REQUIRE(res == 1234);
}

TEST_CASE("never_blocking_oneway_execute", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto neverBlockingExec = enzen::require(exec, enzen::blocking.never);

  int res = -1;

  neverBlockingExec.execute([&res]() {
    res = 1234;
    std::this_thread::sleep_for(2s);
  });

  threadPool.wait();

  REQUIRE(res == 1234);
}

TEST_CASE("bulk_oneway_execute", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto bulkOnewayExec = enzen::require_concept(exec, enzen::bulk_oneway);

  int res[32] = {-1};

  bulkOnewayExec.bulk_execute(
      [&res](enzen ::index idx) { res[idx[0]] = static_cast<int>(idx[0]); },
      enzen::shape{32, 1, 1});

  for (int i = 0; i < 32; ++i) {
    REQUIRE(res[i] == i);
  }
}

TEST_CASE("always_blocking_bulk_oneway_execute", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto bulkOnewayExec = enzen::require_concept(exec, enzen::bulk_oneway);

  auto alwaysBlockingBulkOnewayExec =
      enzen::require(bulkOnewayExec, enzen::blocking.always);

  int res[32] = {-1};

  alwaysBlockingBulkOnewayExec.bulk_execute(
      [&res](enzen ::index idx) {
        res[idx[0]] = static_cast<int>(idx[0]);
        std::this_thread::sleep_for(1s);
      },
      enzen::shape{32, 1, 1});

  for (int i = 0; i < 32; ++i) {
    REQUIRE(res[i] == i);
  }
}

TEST_CASE("never_blocking_bulk_oneway_execute", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto bulkOnewayExec = enzen::require_concept(exec, enzen::bulk_oneway);

  auto neverBlockingBulkOnewayExec =
      enzen::require(bulkOnewayExec, enzen::blocking.never);

  int res[32] = {-1};

  neverBlockingBulkOnewayExec.bulk_execute(
      [&res](enzen ::index idx) {
        res[idx[0]] = static_cast<int>(idx[0]);
        std::this_thread::sleep_for(1s);
      },
      enzen::shape{32, 1, 1});

  threadPool.wait();

  for (int i = 0; i < 32; ++i) {
    REQUIRE(res[i] == i);
  }
}

TEST_CASE("twoway_execute", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto twowayExecutor = enzen::require_concept(exec, enzen::twoway);

  auto fut = twowayExecutor.twoway_execute([]() { return 1234; });

  REQUIRE(fut.get() == 1234);
}

TEST_CASE("schedule", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto lazyExec = enzen::require_concept(exec, enzen::lazy);

  auto s1 = lazyExec.schedule();

  int res = 0;
  enzen::submit(s1, via_receiver<decltype(lazyExec)>{});
}

TEST_CASE("via_and_transform", "thread_pool") {
  auto threadPool =
      enzen::static_thread_pool{std::thread::hardware_concurrency()};

  auto exec = threadPool.executor();

  auto lazyExec = enzen::require_concept(exec, enzen::lazy);

  auto s1 = enzen::just(21);

  auto s2 = enzen::via(lazyExec, s1);

  auto s3 = enzen::transform(s2, [=](int value) { return value * 2; });

  int res = 0;
  submit(s3, value_receiver{&res});

  REQUIRE(res == 42);
}
