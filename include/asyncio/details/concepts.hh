#pragma once

#include <chrono>
#include <type_traits>

namespace asyncio::details {

template<typename T>
constexpr bool chrono_duration_impl = false;

template<typename Rep, typename Period>
constexpr bool chrono_duration_impl<std::chrono::duration<Rep, Period>> = true;

template<typename T>
concept chrono_duration = chrono_duration_impl<std::remove_cvref_t<T>>;

} // namespace asyncio::details
