#pragma once

#include <cassert>
#include <concepts>
#include <memory>
#include <utility>

namespace BadgerEngine {

/// Can only be null when moved
template<typename T>
class NoNull {
public:
    NoNull() = delete;
    NoNull(std::nullptr_t) = delete;

    constexpr NoNull(T v) noexcept(std::is_nothrow_move_constructible_v<T>)
        : m_v(std::move(v))
    {
        assert(m_v);
    }

    template<std::convertible_to<T> O>
    constexpr NoNull(O v) noexcept(std::is_nothrow_move_constructible_v<T>)
        : m_v(std::move(v))
    {
        assert(m_v);
    }

    template<std::convertible_to<T> O>
    constexpr NoNull(NoNull<O> v) noexcept(std::is_nothrow_move_constructible_v<T>)
        : m_v(std::move(v.nullable()))
    {
        assert(m_v);
    }

    constexpr NoNull(const NoNull&) = default;
    constexpr NoNull& operator=(const NoNull& o) = default;

    constexpr NoNull(NoNull&& o) noexcept(std::is_nothrow_move_constructible_v<T>)
        : m_v(std::exchange(o.m_v, nullptr))
    {
        assert(m_v);
    }

    constexpr NoNull& operator=(NoNull&& o)
    {
        m_v = std::exchange(o.m_v, nullptr);
        assert(m_v);
        return *this;
    }

    constexpr decltype(auto) operator->()
        requires(std::is_pointer_v<T>)
    {
        assert(m_v);
        return m_v;
    }

    constexpr decltype(auto) operator->() const
        requires(std::is_pointer_v<T>)
    {
        assert(m_v);
        return m_v;
    }

    constexpr decltype(auto) operator->()
        requires(!std::is_pointer_v<T>)
    {
        assert(m_v);
        return m_v.operator->();
    }

    constexpr decltype(auto) operator->() const
        requires(!std::is_pointer_v<T>)
    {
        assert(m_v);
        return m_v.operator->();
    }

    constexpr auto& operator*()
    {
        assert(m_v);
        return *m_v;
    }

    constexpr const auto& operator*() const
    {
        assert(m_v);
        return *m_v;
    }

    constexpr T& nullable() &
    {
        assert(m_v);
        return m_v;
    }

    // version of value for const lvalues
    constexpr T const& nullable() const&
    {
        assert(m_v);
        return m_v;
    }

    // clang-format off
    // version of value for non-const rvalues... are you bored yet?
    constexpr T&& nullable() &&
        requires(!std::is_pointer_v<T>)
    {
        assert(m_v);
        return std::move(m_v);
    }

    constexpr T nullable() &&
        requires(std::is_pointer_v<T>)
    {
        assert(m_v);
        return m_v;
    }

    constexpr T const&& nullable() const&& = delete;
    // clang-format on

private:
    T m_v;
};

template<typename T>
using Unique = NoNull<std::unique_ptr<T>>;

template<typename T>
using Shared = NoNull<std::shared_ptr<T>>;

template<typename T>
using RawPtr = NoNull<T*>;

}
