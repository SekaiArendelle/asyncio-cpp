module;

#include <utility>
#include <coroutine>
#include <exception>

export module asyncio:task;

namespace asyncio {

export template<typename T>
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

    constexpr ~Task() noexcept {
        if (handle) {
            handle.destroy();
        }
    }

    constexpr Task& operator=(Task const&) = delete;

    constexpr Task& operator=(this Task& self, Task&& other) noexcept {
        self.swap(other);
        return self;
    }

    void swap(this Task& self, Task& other) noexcept {
        static_assert(noexcept(std::ranges::swap(self.handle, other.handle)));
        std::ranges::swap(self.handle, other.handle);
    }

public:
    struct promise_type {
        std::coroutine_handle<> continuation = std::noop_coroutine();

        std::suspend_always initial_suspend() noexcept {
            return {};
        }

        private:
            struct FinalAwaiter {
                static constexpr bool await_ready() noexcept {
                    return false;
                }

                std::coroutine_handle<> await_suspend(this FinalAwaiter const&, handle_type handle) noexcept {
                    return handle.promise().continuation;
                }

                static void await_resume() noexcept {
                }
            };

        public:
        static auto final_suspend() noexcept -> FinalAwaiter {
            return FinalAwaiter{};
        }

        auto get_return_object() noexcept -> Task<T> {
            return handle_type::from_promise(*this);
        }

        static void return_void() noexcept {
        }

        void unhandled_exception() noexcept {
            // TODO this should be handled better, but for now we just terminate the program
            std::terminate();
        }
    };

private:
    struct CoAwaiter {
        handle_type callee;

        static constexpr bool await_ready() noexcept {
            return false;
        }

        auto await_suspend(this CoAwaiter& self, std::coroutine_handle<> caller) noexcept -> std::coroutine_handle<> {
            self.callee.promise().continuation = caller;
            return self.callee;
        }

        static void await_resume() noexcept {
        }
    };

public:
    auto operator co_await(this Task const& self) noexcept -> CoAwaiter {
        return CoAwaiter{self.handle};
    }

    constexpr auto get_handle(this Task const& self) noexcept -> handle_type {
        return self.handle;
    }

    void resume(this Task& self) {
        if (not self.handle) {
            std::terminate();
        }
        if (self.handle.done()) {
            std::terminate();
        }
        self.handle.resume();
    }
};

} // namespace asyncio
