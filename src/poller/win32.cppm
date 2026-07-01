module;

#include <span>

export module asyncio:poller.win32;
import :poller.io_event;

namespace asyncio::details {

export class Win32Poller {
    auto poll(std::optional<std::chrono::system_clock::time_point> timeout) -> std::span<IoEvent> {
        // TODO implement this
        return {};
    }
};

}
