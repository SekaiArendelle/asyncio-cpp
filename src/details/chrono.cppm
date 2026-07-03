module;

#include <cstddef>
#include <chrono>

export module asyncio:details.chrono;

namespace asyncio::details {

template<typename T>
constexpr bool is_small_type_impl =
// MSVC x64 and ARM64EC pass structs by value in registers only when the size
// is exactly 1, 2, 4, or 8 bytes; larger structs go via a hidden pointer.
// (ARM64EC deliberately follows the same x64 rule for emulation compatibility.)
// See: https://learn.microsoft.com/en-us/cpp/build/x64-calling-convention
//      https://learn.microsoft.com/en-us/windows/arm/arm64ec-abi
#if defined(_MSC_VER) && defined(_WIN64) && \
    (defined(_M_X64) || defined(__x86_64__) || defined(__amd64__) || defined(_M_ARM64EC))
    sizeof(T) <= sizeof(std::size_t);
// SysV x86-64 ABI can split up to 16-byte aggregates across two registers,
// so the threshold is relaxed to sizeof(std::size_t) * 2.
// See: https://www.uclibc.org/docs/psABI-x86_64.pdf
#else
    sizeof(T) <= sizeof(std::size_t) * 2;
#endif

template<typename T>
constexpr bool small_chrono_duration_impl = false;

template<typename Rep, typename Period>
    requires (is_small_type_impl<Rep>)
constexpr bool small_chrono_duration_impl<std::chrono::duration<Rep, Period>> = true;

export template<typename T>
concept small_chrono_duration = small_chrono_duration_impl<std::remove_cvref_t<T>>;

template<typename T>
constexpr bool huge_chrono_duration_impl = false;

template<typename Rep, typename Period>
    requires (not is_small_type_impl<Rep>)
constexpr bool huge_chrono_duration_impl<std::chrono::duration<Rep, Period>> = true;

export template<typename T>
concept huge_chrono_duration = huge_chrono_duration_impl<std::remove_cvref_t<T>>;

} // namespace asyncio::details
