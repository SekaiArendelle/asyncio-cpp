import asyncio;

#include <coroutine>
#include <exception>

auto func() -> asyncio::Task<int> {
    co_return 1;
}

auto run() -> asyncio::Task<void> {
    auto result = co_await func();
    if (result != 1) {
        std::terminate();
    }
}

int main() {
    auto task = run();
    task.resume();
}
