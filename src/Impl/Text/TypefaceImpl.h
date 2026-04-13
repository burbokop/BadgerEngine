#pragma once

#include <freetype/freetype.h>
#include <ft2build.h>
#include <utility>

namespace BadgerEngine {

class TypefaceImpl {

public:
    TypefaceImpl(const TypefaceImpl&) = delete;
    TypefaceImpl(TypefaceImpl&&) = delete;
    TypefaceImpl& operator=(const TypefaceImpl&) = delete;
    TypefaceImpl& operator=(TypefaceImpl&&) = delete;

    TypefaceImpl(
        FT_Library ft,
        FT_Face face)
        : m_ft(std::move(ft))
        , m_face(std::move(face))
    {
    }

    ~TypefaceImpl()
    {
        ::FT_Done_Face(m_face);
        ::FT_Done_FreeType(m_ft);
    }

    const FT_Library& ft() const { return m_ft; }
    const FT_Face& face() const { return m_face; }

private:
    FT_Library m_ft;
    FT_Face m_face;
};

}
