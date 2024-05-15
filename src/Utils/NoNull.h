#pragma once

#include <cassert>
#include <concepts>
#include <memory>
#include <utility>

namespace BadgerEngine {

// template<typename T, typename O>
// concept Constructible = std::is_constructible_v<T, O>;

/// Can only be null when moved
template<typename T>
class NoNull {
public:
    NoNull() = delete;
    NoNull(std::nullptr_t) = delete;

    NoNull(T v) noexcept(std::is_nothrow_move_constructible_v<T>)
        : m_v(std::move(v))
    {
        assert(m_v);
    }

    template<std::convertible_to<T> O>
    NoNull(O v) noexcept(std::is_nothrow_move_constructible_v<T>)
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

    NoNull(const NoNull&) = default;
    NoNull& operator=(const NoNull& o) = default;

    NoNull(NoNull&& o) noexcept(std::is_nothrow_move_constructible_v<T>)
        : m_v(std::exchange(o.m_v, nullptr))
    {
        assert(m_v);
    }

    NoNull& operator=(NoNull&& o)
    {
        m_v = std::exchange(o.m_v, nullptr);
        assert(m_v);
        return *this;
    }

    auto operator->()
    {
        assert(m_v);
        return m_v.operator->();
    }

    const auto operator->() const
    {
        assert(m_v);
        return m_v.operator->();
    }

    auto& operator*()
    {
        assert(m_v);
        return *m_v;
    }

    const auto& operator*() const
    {
        assert(m_v);
        return *m_v;
    }

    // template<class Self>
    // constexpr auto&& nullable(this Self&& self)
    // {
    //     assert(m_v);
    //     return m_v;
    // }

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

    // version of value for non-const rvalues... are you bored yet?
    constexpr T&& nullable() &&
    {
        assert(m_v);
        return m_v;
    }

    // you sure are by this point
    constexpr T const&& nullable() const&&
    {
        assert(m_v);
        return m_v;
    }

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
