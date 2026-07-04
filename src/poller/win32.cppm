module;

#include <span>
#include <coroutine>
#include <windows.h>

export module asyncio:poller.win32;
import :details.chrono;
import :poller.io_event;

namespace asyncio {

class EventLoop;

}

namespace asyncio::poller {

export class Win32Socket {
public:
    SOCKET socket;
    EventLoop& loop;

private:
    struct ReadAwaiter {};

public:
    // constexpr auto read
};

export class Win32Poller {
    HANDLE iocp_handle;

public:
    auto poll(details::small_chrono_duration auto timeout) -> std::span<IoEvent> { // TODO maybe std::array instead of std::span?
        // TODO implement this
        return {};
    }
};

} // namespace asyncio::poller
