import asyncio;

#include <print>
#include <cstdio>
#include <coroutine>

auto bar() -> asyncio::Task<void> {
    std::puts("[bar] Doing some work in bar...");
    co_return;
}

auto baz() -> asyncio::Task<int> {
    std::puts("[baz] Doing some work in baz...");
    co_return 1;
}

auto foo() -> asyncio::Task<void> {
    std::puts("[foo] Calling bar...");
    co_await bar();
    // equivalently, you can do:
    // auto task = bar();
    // task.resume();
    std::puts("[foo] Back from bar!");

    std::puts("[foo] Calling baz...");
    int result = co_await baz();
    // equivalently, you can do:
    // auto task = baz();
    // task.resume();
    // int result = task.result();
    std::println("[foo] Back from baz! Result: {}", result);
    co_return;
}

int main() {
    std::puts("[main] Starting foo...");
    auto task = foo();
    task.resume();
    std::puts("[main] Finished foo!");

    return 0;
}
