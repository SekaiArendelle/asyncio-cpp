module;

#include <span>
#include <optional>
#include <concepts>
#include <coroutine>

export module asyncio:poller;
export import asyncio:poller.io_event;

namespace asyncio {

export template<typename T>
concept is_poller = requires(T poller, details::small_chrono_duration auto timeout) {
    { p.poll(timeout) } -> std::same_as<std::span<poller::IoEvent>>;
};

#if 1
import asyncio:poller.win32;
export using NativePoller = poller::Win32Poller;
#else
#endif

static_assert(is_poller<NativePoller>, "Poller must satisfy the is_poller concept");

} // namespace asyncio
