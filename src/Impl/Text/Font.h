#pragma once

#include "../../Model/Texture.h"
#include "../../Utils/Error.h"

namespace BadgerEngine {

class Typeface;
class TypefaceImpl;

class Font {
    friend Typeface;

public:
    Expected<SharedTexture> glyph(char c) const;
    Expected<SharedTexture> text(const std::string& str) const;

private:
    Font(Shared<TypefaceImpl> impl, std::size_t size)
        : m_impl(std::move(impl))
        , m_size(std::move(size))
    {
    }

private:
    Shared<TypefaceImpl> m_impl;
    std::size_t m_size;
};

}
