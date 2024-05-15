#pragma once

#include <ranges>

namespace BadgerEngine {

template<template<typename, typename> typename T>
struct CollectImpl {
    template<std::ranges::range R>
    friend T<std::ranges::range_value_t<R>, std::allocator<std::ranges::range_value_t<R>>> operator|(R&& r, CollectImpl)
    {
        return T(std::ranges::begin(r), std::ranges::end(r));
    }
};

template<template<typename, typename> typename T>
constexpr CollectImpl<T> Collect;

}
