import asyncio;

#include <coroutine>
#include <exception>

auto func() -> asyncio::Task<int> {
    co_return 1;
}

int main() {
    auto task = func();
    task.resume();
    if (task.result() != 1) {
        std::terminate();
    }
}
