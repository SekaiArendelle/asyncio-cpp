#pragma once

#include <coroutine>
#include <exception>

namespace asyncio {

template<typename T>
class Task {
public:
    class promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

private:
    handle_type handle;

public:
    constexpr Task(handle_type handle) noexcept
        : handle(handle) {
    }

    constexpr Task(Task const&) = delete;

    constexpr Task(Task&& other) noexcept
        : handle(other.handle) {
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

} // namespace asyncio
