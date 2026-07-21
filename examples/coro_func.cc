import asyncio;

#include <cstdio>
#include <coroutine>

auto bar() -> asyncio::Task<void> {
    std::puts("[bar] Doing some work in bar...");
    co_return;
}

auto foo() -> asyncio::Task<void> {
    std::puts("[foo] Calling bar...");
    co_await bar();
    // equivalently, you can do:
    // auto task = bar();
    // task.resume();
    std::puts("[foo] Back from bar!");
    co_return;
}

int main() {
    std::puts("[main] Starting foo...");
    auto task = foo();
    task.resume();
    std::puts("[main] Finished foo!");

    return 0;
}
