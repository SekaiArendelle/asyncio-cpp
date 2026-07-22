#ifdef NDEBUG
    #error "Tests should be compiled in debug mode"
#endif

import asyncio;

#include <coroutine>
#include <exception>

auto int_task() -> asyncio::Task<int> {
    co_return 42;
}

auto void_task() -> asyncio::Task<void> {
    co_return;
}

auto chained_task() -> asyncio::Task<void> {
    auto r = co_await int_task();
    if (r != 42) {
        std::terminate();
    }
}

int main() {
    // Task<int> before and after resume
    auto t1 = int_task();
    if (t1.done()) {
        std::terminate();
    }
    t1.resume();
    if (!t1.done()) {
        std::terminate();
    }
    if (t1.result() != 42) {
        std::terminate();
    }

    // Task<void> before and after resume
    auto t2 = void_task();
    if (t2.done()) {
        std::terminate();
    }
    t2.resume();
    if (!t2.done()) {
        std::terminate();
    }

    // Moved-from task reports done
    auto t3 = int_task();
    auto t4 = std::move(t3);
    if (!t3.done()) {
        std::terminate();
    }
    if (t4.done()) {
        std::terminate();
    }
    t4.resume();
    if (!t4.done()) {
        std::terminate();
    }

    // chained task
    auto t5 = chained_task();
    if (t5.done()) {
        std::terminate();
    }
    t5.resume();
    if (!t5.done()) {
        std::terminate();
    }
}
