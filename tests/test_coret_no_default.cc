import asyncio;

#include <coroutine>
#include <exception>

struct Widget {
    int value;
    explicit Widget(int v) : value(v) {}
    Widget() = delete;
};

auto func() -> asyncio::Task<Widget> {
    co_return Widget{1};
}

auto run() -> asyncio::Task<void> {
    auto result = co_await func();
    if (result.value != 1) {
        std::terminate();
    }
}

int main() {
    auto task = run();
    task.resume();
}
