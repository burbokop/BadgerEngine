#pragma once

#include "Font.h"
#include <span>

namespace BadgerEngine {

class Typeface {
public:
    static Expected<Typeface> parse(std::span<const std::uint8_t> bytes);

    Expected<Font> font(std::size_t size) const;

private:
    Typeface(Shared<TypefaceImpl> impl)
        : m_impl(std::move(impl))
    {
    }

private:
    Shared<TypefaceImpl> m_impl;
};

}
