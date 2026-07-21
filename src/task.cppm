module;

#include <memory>
#include <optional>
#include <utility>
#include <coroutine>
#include <exception>
#include <type_traits>

export module asyncio:task;

namespace asyncio {

export template<typename T>
class Task {
    class void_task_promise_type;
    class typed_task_promise_type;
public:
    using promise_type = std::conditional_t<std::is_void_v<T>, void_task_promise_type, typed_task_promise_type>;
    using handle_type = std::coroutine_handle<promise_type>;

private:
    handle_type handle;

public:
    constexpr Task(handle_type handle) noexcept
        : handle(handle) {
    }

    constexpr Task(Task<T> const&) = delete;

    constexpr Task(Task<T>&& other) noexcept
        : handle(other.handle) {
        other.handle = nullptr;
    }

    constexpr ~Task() noexcept {
        if (handle) {
            handle.destroy();
        }
    }

    constexpr Task<T>& operator=(Task<T> const&) = delete;

    constexpr Task<T>& operator=(this Task<T>& self, Task<T>&& other) noexcept {
        self.swap(other);
        return self;
    }

    void swap(this Task<T>& self, Task<T>& other) noexcept {
        static_assert(noexcept(std::ranges::swap(self.handle, other.handle)));
        std::ranges::swap(self.handle, other.handle);
    }

private:
    struct PromiseFinalSuspendAwaiter {
        static constexpr bool await_ready() noexcept {
            return false;
        }

        static auto await_suspend(handle_type handle) noexcept -> std::coroutine_handle<> {
            return handle.promise().continuation;
        }

        static void await_resume() noexcept {
        }
    };

    struct void_task_promise_type {
        std::coroutine_handle<> continuation = std::noop_coroutine();

        std::suspend_always initial_suspend() noexcept {
            return {};
        }

        static auto final_suspend() noexcept -> PromiseFinalSuspendAwaiter {
            return PromiseFinalSuspendAwaiter{};
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

    /// Promise type for typed (non-void) asyncio::Task<T>.
    ///
    /// std::optional<T> is used for the result value so that T is not
    /// required to be default-constructible.  The optional is empty when
    /// the coroutine is created and is populated by return_value() when
    /// the coroutine completes.  This avoids the need for manual lifetime
    /// management via unions and placement-new.
    struct typed_task_promise_type {
        std::coroutine_handle<> continuation = std::noop_coroutine();

        /// Coroutine result value.
        ///
        /// Default-constructs as empty (no T constructed).  This eliminates
        /// the requirement that T be default-constructible, which is
        /// necessary for types that can only be created with specific
        /// arguments.
        std::optional<T> value;

        std::suspend_always initial_suspend() noexcept {
            return {};
        }

        static auto final_suspend() noexcept -> PromiseFinalSuspendAwaiter {
            return PromiseFinalSuspendAwaiter{};
        }

        auto get_return_object() noexcept -> Task<T> {
            return handle_type::from_promise(*this);
        }

        void return_value(T value_) noexcept {
            value = std::move(value_);
        }

        void unhandled_exception() noexcept {
            // TODO this should be handled better, but for now we just terminate the program
            std::terminate();
        }
    };

private:
    struct VoidTaskCoAwaiter {
        handle_type callee;

        static constexpr bool await_ready() noexcept {
            return false;
        }

        auto await_suspend(this VoidTaskCoAwaiter& self, std::coroutine_handle<> caller) noexcept -> std::coroutine_handle<> {
            self.callee.promise().continuation = caller;
            return self.callee;
        }

        static void await_resume() noexcept {
        }
    };

    struct TypedTaskCoAwaiter {
        handle_type callee;

        static constexpr bool await_ready() noexcept {
            return false;
        }

        auto await_suspend(this TypedTaskCoAwaiter& self, std::coroutine_handle<> caller) noexcept -> std::coroutine_handle<> {
            self.callee.promise().continuation = caller;
            return self.callee;
        }

        auto await_resume() noexcept -> T {
            return std::move(*this->callee.promise().value);
        }
    };

public:
    auto operator co_await(this Task<T> const& self) noexcept {
        if constexpr (std::is_void_v<T>) {
            return VoidTaskCoAwaiter{self.handle};
        } else {
            return TypedTaskCoAwaiter{self.handle};
        }
    }

    constexpr auto get_handle(this Task<T> const& self) noexcept -> handle_type {
        return self.handle;
    }

    void resume(this Task<T>& self) {
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
