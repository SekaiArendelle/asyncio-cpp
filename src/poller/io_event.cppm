module;

#include <coroutine>

export module asyncio:poller.io_event;

namespace asyncio::poller {

export struct IoEvent {
    std::coroutine_handle<> handle;
    size_t bytes_transferred;
    int error_code;        // 0 = success
};

}
