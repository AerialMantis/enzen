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

#define ENZEN_VERBOSE
#include <execution>

using namespace std::chrono_literals;

TEST_CASE("constructor_destructor", "static_thread_pool") {
  enzen::static_thread_pool threadPool{4};

  REQUIRE(true);
}

TEST_CASE("start_and_wait", "static_thread_pool") {
  enzen::static_thread_pool threadPool{4};

  threadPool.wait();

  REQUIRE(true);
}

TEST_CASE("start_and_stop", "static_thread_pool") {
  enzen::static_thread_pool threadPool{4};

  threadPool.stop();

  REQUIRE(true);
}

TEST_CASE("executor", "static_thread_pool") {
  enzen::static_thread_pool threadPool{4};

  auto exec = threadPool.executor();

  bool taskRun = false;

  exec.execute([&taskRun]() { taskRun = true; });

  threadPool.wait();

  REQUIRE(taskRun);
}

TEST_CASE("concurrent_threads", "static_thread_pool") {
  auto numThreads = std::thread::hardware_concurrency();

  enzen::static_thread_pool threadPool{numThreads};

  auto exec = threadPool.executor();

  std::vector<bool> tasksRun(numThreads, false);

  for (size_t i = 0; i < numThreads; ++i) {
    exec.execute([=, &tasksRun]() { tasksRun[i] = true; });
  }

  threadPool.wait();

  for (auto &taskRun : tasksRun) {
    REQUIRE(taskRun);
  }
}

TEST_CASE("thread_saturation", "static_thread_pool") {
  auto numThreads = size_t{100};
  auto numTasks = size_t{20};

  enzen::static_thread_pool threadPool{numThreads};

  auto exec = threadPool.executor();

  std::vector<bool> tasksRun(numTasks, false);

  for (size_t i = 0; i < numTasks; ++i) {
    exec.execute([=, &tasksRun]() { tasksRun[i] = true; });
  }

  threadPool.wait();

  for (auto &taskRun : tasksRun) {
    REQUIRE(taskRun);
  }
}

TEST_CASE("high_number_of_tasks", "static_thread_pool") {
  auto numThreads = size_t{8};
  auto numTasks = size_t{100};

  enzen::static_thread_pool threadPool{numThreads};

  auto exec = threadPool.executor();

  std::vector<bool> tasksRun(numTasks, false);

  for (size_t i = 0; i < numTasks; ++i) {
    exec.execute([=, &tasksRun]() { tasksRun[i] = true; });
  }

  threadPool.wait();

  for (auto &taskRun : tasksRun) {
    REQUIRE(taskRun);
  }
}

TEST_CASE("long_running_tasks", "static_thread_pool") {
  auto numThreads = std::thread::hardware_concurrency();
  auto numTasks = numThreads;

  enzen::static_thread_pool threadPool{numThreads};

  auto exec = threadPool.executor();

  std::atomic<size_t> tasksComplete = 0;

  for (size_t i = 0; i < numTasks; ++i) {
    exec.execute([&]() {
      std::this_thread::sleep_for(5s);
      tasksComplete++;
    });
  }

  threadPool.wait();

  REQUIRE(tasksComplete == numTasks);
}