#pragma once

#include <coroutine>
#include <vector>
#include <queue>
#include <chrono>

#include "details/concepts.hh"

namespace asyncio {

// template<typename T>
// class Future {};

/**
 * @brief a Task is a running Future
 */
template<typename T>
class Task {
public:
    class promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

private:
    handle_type handle;

public:
    constexpr Task(handle_type handle) noexcept : handle(handle) {
    }

    constexpr Task(Task const&) = delete;
    constexpr Task(Task&& other) noexcept : handle(other.handle) {
        other.handle = nullptr;
    }

    constexpr ~Task() {
        if (handle) {
            handle.destroy();
        }
    }

public:
    class promise_type {
    public:
        std::suspend_always initial_suspend() noexcept {
            return {};
        }

        std::suspend_always final_suspend() noexcept {
            return {};
        }

        Task<T> get_return_object() noexcept {
            return handle_type::from_promise(*this);
        }

        void return_void() noexcept {
        }

        void unhandled_exception() noexcept {
            // TODO this should be handled better, but for now we just terminate the program
            std::terminate();
        }
    };

    constexpr auto get_handle(this Task const& self) noexcept -> handle_type {
        return self.handle;
    }
};

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

    constexpr void run() {
        while (not ready_queue.empty() or not timers.empty()) {
            // Process ready tasks
            while (not ready_queue.empty()) {
                auto handle = ready_queue.front();
                ready_queue.pop_front();
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
