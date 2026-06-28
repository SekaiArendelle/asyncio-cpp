#pragma once

#include <cassert>
#include <thread>
#include <coroutine>
#include <vector>
#include <queue>
#include <chrono>

#include "task.hh"
#include "details/concepts.hh"

namespace asyncio {

class EventLoop {
    struct QueueElement {
        std::coroutine_handle<> handle;
        std::chrono::steady_clock::time_point time;
    };

    struct TimerCompare {
        static bool operator()(QueueElement const& a, QueueElement const& b) noexcept(noexcept(a.time > b.time)) {
            return a.time > b.time; // Compare based on time for priority queue
        }
    };

    std::priority_queue<QueueElement, std::vector<QueueElement>, TimerCompare> timers;
    std::deque<std::coroutine_handle<>> ready_queue;

public:
    EventLoop() = default;

    /**
     * @note Only lvalue references is allowed, otherwise it would be a UAF.
     */
    template<typename T>
    constexpr void create_task(Task<T>& task) {
        ready_queue.push_back(task.get_handle());
    }

    /**
     * @brief Run the event loop until no tasks or timers remain.
     *
     * Marked `constexpr` for safety rather than compile-time evaluation.
     * C++23 (P2448) permits non-constexpr statements inside a constexpr
     * function as long as they are never reached during constant evaluation.
     * The constexpr specifier enables the compiler to enforce a stricter
     * subset of the language that eliminates several categories of C
     * legacy undefined behaviour.
     */
    constexpr void run() {
        while (not ready_queue.empty() or not timers.empty()) {
            // Process ready tasks
            auto current_ready_queue = std::move(ready_queue);
            for (auto handle : current_ready_queue) {
                assert(not handle.done() && "Task should not be done when resuming");
                handle.resume();
            }

            // Process timers
            if (not timers.empty()) {
                auto now = std::chrono::steady_clock::now();
                while (not timers.empty() and timers.top().time <= now) {
                    auto handle = timers.top().handle;
                    timers.pop();
                    ready_queue.push_back(handle);
                }
                if (not timers.empty()) {
                    std::this_thread::sleep_until(timers.top().time);
                }
            }
        }
    }

private:
    class SleepAwaiter {
    public:
        std::chrono::steady_clock::time_point wake_time;
        EventLoop& loop;

        bool await_ready(this SleepAwaiter const& self) noexcept(noexcept(std::chrono::steady_clock::now() >=
                                                                          self.wake_time)) {
            return std::chrono::steady_clock::now() >= self.wake_time;
        }

        void await_suspend(this SleepAwaiter const& self, std::coroutine_handle<> handle) {
            // TODO check exception safety for this method
            self.loop.timers.push({handle, self.wake_time});
        }

        void await_resume(this SleepAwaiter const&) noexcept {
        }
    };

public:
    /**
     * @brief Sleep for a given duration
     * @param duration The duration to sleep for
     * @return An awaitable that will resume after the given duration
     * @note Only lvalue references make sense, cause a temporary EventLoop can not resume anything.
     */
    [[nodiscard]]
    constexpr auto sleep(this EventLoop& self, details::chrono_duration auto duration) {
        return SleepAwaiter{std::chrono::steady_clock::now() + duration, self};
    }
};

} // namespace asyncio
