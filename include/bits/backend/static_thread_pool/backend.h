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

#ifndef __ENZEN_STATIC_THREAD_POOL_BACKEND_H__
#define __ENZEN_STATIC_THREAD_POOL_BACKEND_H__

#include <atomic>
#include <condition_variable>

#include <bits/concurrent_queue.h>
#include <bits/debug.h>

namespace enzen::detail {

enum class thread_pool_status : int { idle, running, shutdown, waiting, error };

class thread_pool_backend {
 public:
  template <typename Task, typename Function>
  using transform_task_t = enzen::thread_pool_transform_task<Task, Function>;

  template <typename Executor, typename Task>
  using via_task_t = enzen::thread_pool_via_task<Executor, Task>;

  using executor_t = basic_executor<detail::thread_pool_backend,
                                    detail::executor_interface::oneway, void>;

  using sub_executor_t =
      basic_executor<detail::thread_pool_backend,
                     detail::executor_interface::oneway, void>;

  thread_pool_backend(std::size_t numThreads) : numThreads_{numThreads} {
    threadPoolStatus_ = thread_pool_status::idle;
    runningTasks_ = 0;

    workerThreads_.reserve(numThreads);
  }

  thread_pool_backend(const thread_pool_backend &) = delete;
  thread_pool_backend(thread_pool_backend &&) = delete;
  thread_pool_backend &operator=(const thread_pool_backend &) = delete;
  thread_pool_backend *operator=(thread_pool_backend &&) = delete;

  void start() {
    if (threadPoolStatus_ == thread_pool_status::idle) {
      auto workerFunc = [this](int threadPoolId) {
        auto threadId = std::this_thread::get_id();

        ENZEN_DEBUG_LOG(indent((threadPoolId + 1) * 9), "startup ")

        while (true) {
          {
            auto deferredLock =
                std::unique_lock<std::mutex>{this->signalWorkersMutex_};
            signalWorkersCV_.wait(deferredLock, [&]() {
              return this->is_thread_wakeup_condition_met();
            });
            ENZEN_DEBUG_LOG(indent((threadPoolId + 1) * 9), "wake up ")
            deferredLock.unlock();
          }

          // No work left
          if (this->threadPoolStatus_ == thread_pool_status::waiting &&
              this->concurrentQueue_.empty()) {
            ENZEN_DEBUG_LOG(indent((threadPoolId + 1) * 9), "wait sat") {
              auto lock = std::lock_guard<std::mutex>{signalHostMutex_};
              signalHostCV_.notify_all();
            }
          }

          // Exit condition
          if (concurrentQueue_.empty() &&
              this->threadPoolStatus_ == thread_pool_status::shutdown) {
            ENZEN_DEBUG_LOG(indent((threadPoolId + 1) * 9), "exit sat")
            break;
          }

          auto currentTask = std::function<void()>{};

          if (!this->concurrentQueue_.empty()) {
            ENZEN_DEBUG_LOG(indent((threadPoolId + 1) * 9), "inc task")
            this->runningTasks_++;
            auto res = concurrentQueue_.try_pop(currentTask);
            if (res) {
              ENZEN_DEBUG_LOG(indent((threadPoolId + 1) * 9), "pop     ")
            } else {
              ENZEN_DEBUG_LOG(indent((threadPoolId + 1) * 9), "pop fail")
            }

            if (currentTask) {
              ENZEN_DEBUG_LOG(indent((threadPoolId + 1) * 9), "new task")
              currentTask();
              ENZEN_DEBUG_LOG(indent((threadPoolId + 1) * 9), "end task")
            }
            ENZEN_DEBUG_LOG(indent((threadPoolId + 1) * 9), "dec task")
            this->runningTasks_--;
          } else {
            ENZEN_DEBUG_LOG(indent((threadPoolId + 1) * 9), "no tasks")
          }
        }

        ENZEN_DEBUG_LOG(indent((threadPoolId + 1) * 9), "shutdown")
      };

      for (int i = 0; i < numThreads_; ++i) {
        workerThreads_.emplace_back(workerFunc, i);
      }

      threadPoolStatus_ = thread_pool_status::running;
    }
  }

  void attach() {}

  void stop() {
    if (threadPoolStatus_ == thread_pool_status::running ||
        threadPoolStatus_ == thread_pool_status::waiting) {
      threadPoolStatus_ = thread_pool_status::shutdown;
      {
        ENZEN_DEBUG_LOG(indent(0), "-> stop ")
        auto lock = std::lock_guard<std::mutex>{signalWorkersMutex_};
        signalWorkersCV_.notify_all();
      }

      threadPoolStatus_ = thread_pool_status::idle;
    }
  }

  void wait() {
    if (threadPoolStatus_ == thread_pool_status::running) {
      {
        ENZEN_DEBUG_LOG(indent(0), "-> wait ")
        auto lock = std::lock_guard<std::mutex>{signalWorkersMutex_};
        threadPoolStatus_ = thread_pool_status::waiting;
        signalWorkersCV_.notify_all();
      }

      {
        auto deferredLock =
            std::unique_lock<std::mutex>{this->signalHostMutex_};
        signalHostCV_.wait(deferredLock, [&]() {
          ENZEN_DEBUG_LOG(indent(0), "?? done ")
          return this->is_wait_complete_condition_met();
        });
        deferredLock.unlock();
        ENZEN_DEBUG_LOG(indent(0), "<- done ")
      }

      {
        auto lock = std::lock_guard<std::mutex>{signalWorkersMutex_};
        threadPoolStatus_ = thread_pool_status::running;
        ENZEN_DEBUG_LOG(indent(0), "-> run  ")
      }
    }
  }

  void join() {
    {
      ENZEN_DEBUG_LOG(indent(0), "-> shut ")
      auto lock = std::lock_guard<std::mutex>{signalWorkersMutex_};
      threadPoolStatus_ = thread_pool_status::shutdown;
      signalWorkersCV_.notify_all();
    }

    ENZEN_DEBUG_LOG(indent(0), "= join =")
    for (auto &worker : workerThreads_) {
      if (worker.joinable()) {
        worker.join();
      }
    }

    {
      ENZEN_DEBUG_LOG(indent(0), "-> idle ")
      auto lock = std::lock_guard<std::mutex>{signalWorkersMutex_};
      threadPoolStatus_ = thread_pool_status::idle;
    }
  }

  template <typename KernalName, typename Function>
  void execute(Function &&f, detail::executor_blocking blockingSemantics) {
    this->enqueue_task(f);
    // TODO(Gordon): Should only wait on this task, not the context.
    if (blockingSemantics == detail::executor_blocking::always ||
        blockingSemantics == detail::executor_blocking::possibly) {
      this->wait();
    }
  }

  template <typename KernalName, typename Function>
  void bulk_execute(Function &&f, shape shape,
                    detail::executor_blocking blockingSemantics) {
    this->bulk_enqueue_task(f, shape);
    // TODO(Gordon): Should only wait on this task, not the context.
    if (blockingSemantics == detail::executor_blocking::always ||
        blockingSemantics == detail::executor_blocking::possibly) {
      this->wait();
    }
  }

  template <typename KernalName, typename Function>
  auto twoway_execute(Function &&f,
                      detail::executor_blocking blockingSemantics) {
    using return_type =
        std::remove_cv_t<std::decay_t<decltype(std::declval<Function &&>()())>>;

    auto prom = promise<return_type>{};
    auto fut = prom.get_future();

    this->enqueue_task(
        [f = std::forward<Function &&>(f), prom = std::move(prom)]() mutable {
          prom.set_value(f());
        });
    return fut;
  }

 private:
  std::size_t num_workers() const noexcept { return numThreads_; }

  template <typename Function>
  void enqueue_task(Function &&f) {
    if (threadPoolStatus_ == thread_pool_status::running) {
      concurrentQueue_.push(f);
      {
        auto lock = std::lock_guard<std::mutex>{signalWorkersMutex_};
        signalWorkersCV_.notify_one();
      }
    } else {
      throw std::exception("Failed to schedule task, thread pool not running.");
    }
  }

  template <typename Function>
  void bulk_enqueue_task(Function &&f, shape shape) {
    if (threadPoolStatus_ == thread_pool_status::running) {
      for (size_t i = 0; i < shape[0]; ++i) {
        for (size_t j = 0; j < shape[1]; ++j) {
          for (size_t k = 0; k < shape[2]; ++k) {
            auto idx = enzen::index{shape, i, j, k};
            concurrentQueue_.push([f, idx]() { f(idx); });
          }
        }
      }
      {
        auto lock = std::lock_guard<std::mutex>{signalWorkersMutex_};
        signalWorkersCV_.notify_one();
      }
    } else {
      throw std::exception("Failed to schedule task, thread pool not running.");
    }
  }

  bool is_thread_wakeup_condition_met() const noexcept {
    if (this->threadPoolStatus_ == thread_pool_status::running) {
      return !concurrentQueue_.empty();
    } else {
      return (this->threadPoolStatus_ == thread_pool_status::waiting ||
              this->threadPoolStatus_ == thread_pool_status::shutdown);
    }
  }

  bool is_wait_complete_condition_met() const noexcept {
    return (this->concurrentQueue_.empty() && this->runningTasks_ == 0);
  }

  std::atomic<thread_pool_status> threadPoolStatus_;
  std::atomic<size_t> runningTasks_;
  std::size_t numThreads_;
  std::vector<std::thread> workerThreads_;
  concurrent_queue<std::function<void()>> concurrentQueue_;
  std::condition_variable signalWorkersCV_;
  std::condition_variable signalHostCV_;
  std::mutex signalWorkersMutex_;
  std::mutex signalHostMutex_;
};

}  // namespace enzen::detail

#endif  // __ENZEN_STATIC_THREAD_POOL_BACKEND_H__