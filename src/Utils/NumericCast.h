#pragma once

#include <limits>
#include <optional>
#include <type_traits>
#include <utility>

namespace BadgerEngine {

template<std::integral A, std::integral B>
constexpr bool cmpLess(A a, B b) noexcept
{
    return std::cmp_less(a, b);
}

template<std::floating_point A, std::integral B>
constexpr bool cmpLess(A a, B b) noexcept
{
    return a < static_cast<A>(b);
}

template<std::integral A, std::floating_point B>
constexpr bool cmpLess(A a, B b) noexcept
{
    return static_cast<B>(a) < b;
}

template<std::floating_point A, std::floating_point B>
constexpr bool cmpLess(A a, B b) noexcept
{
    if constexpr (sizeof(A) > sizeof(B)) {
        return a < static_cast<A>(b);
    } else {
        return static_cast<B>(a) < b;
    }
}

template<std::integral A, std::integral B>
constexpr bool cmpGreater(A a, B b) noexcept
{
    return std::cmp_greater(a, b);
}

template<std::floating_point A, std::integral B>
constexpr bool cmpGreater(A a, B b) noexcept
{
    return a > static_cast<A>(b);
}

template<std::integral A, std::floating_point B>
constexpr bool cmpGreater(A a, B b) noexcept
{
    return static_cast<B>(a) > b;
}

template<std::floating_point A, std::floating_point B>
constexpr bool cmpGreater(A a, B b) noexcept
{
    if constexpr (sizeof(A) > sizeof(B)) {
        return a > static_cast<A>(b);
    } else {
        return static_cast<B>(a) > b;
    }
}

template<typename To, typename From>
    requires(std::is_arithmetic_v<To> && std::is_arithmetic_v<From>)
[[nodiscard]] constexpr std::optional<To> numericCast(From from) noexcept
{
    if constexpr (!std::is_same_v<To, From>) {
        if (cmpLess(from, std::numeric_limits<To>::lowest()) || cmpGreater(from, std::numeric_limits<To>::max())) [[unlikely]] {
            return std::nullopt;
        }
    }

    return static_cast<To>(from);
}

}
