#pragma once

#include <limits>
#include <optional>
#include <type_traits>
#include <utility>

namespace BadgerEngine {

template<std::integral A, std::integral B>
bool cmpLess(A a, B b)
{
    return std::cmp_less(a, b);
}

template<std::floating_point A, std::integral B>
bool cmpLess(A a, B b)
{
    return a < static_cast<A>(b);
}

template<std::integral A, std::floating_point B>
bool cmpLess(A a, B b)
{
    return static_cast<B>(a) < b;
}

template<std::floating_point A, std::floating_point B>
bool cmpLess(A a, B b)
{
    if (sizeof(A) > sizeof(B)) {
        return a < static_cast<A>(b);
    } else {
        return static_cast<B>(a) < b;
    }
}

template<std::integral A, std::integral B>
bool cmpGreater(A a, B b)
{
    return std::cmp_greater(a, b);
}

template<std::floating_point A, std::integral B>
bool cmpGreater(A a, B b)
{
    return a > static_cast<A>(b);
}

template<std::integral A, std::floating_point B>
bool cmpGreater(A a, B b)
{
    return static_cast<B>(a) > b;
}

template<std::floating_point A, std::floating_point B>
bool cmpGreater(A a, B b)
{
    if (sizeof(A) > sizeof(B)) {
        return a > static_cast<A>(b);
    } else {
        return static_cast<B>(a) > b;
    }
}

template<typename To, typename From>
    requires(std::is_arithmetic_v<To> && std::is_arithmetic_v<From>)
[[nodiscard]] std::optional<To> numericCast(From from) noexcept
{
    if (!std::is_same_v<To, From>
        && (cmpLess(from, std::numeric_limits<To>::min()) || cmpGreater(from, std::numeric_limits<To>::max()))) [[unlikely]] {
        return std::nullopt;
    }

    return static_cast<To>(from);
}
}
