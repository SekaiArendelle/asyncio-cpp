#include <asyncio/asyncio.hh>

using namespace std::chrono_literals;

auto foo(asyncio::EventLoop& loop) -> asyncio::Task<void> {
    co_await loop.sleep(1s);
}

int main() {
    asyncio::EventLoop event_loop{};
    auto task = foo(event_loop);
    event_loop.create_task(task);
    event_loop.run();

    return 0;
}
