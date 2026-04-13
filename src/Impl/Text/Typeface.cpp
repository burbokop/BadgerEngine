#include "Typeface.h"

#include "../Utils/NumericCast.h"

#ifdef BADGER_ENGINE_FREETYPE
#include "TypefaceImpl.h"
#endif

namespace BadgerEngine {

namespace {
}

Expected<Typeface> Typeface::parse(std::span<const std::uint8_t> bytes)
{
#ifdef BADGER_ENGINE_FREETYPE
    FT_Library ft;
    if (::FT_Init_FreeType(&ft)) {
        return unexpected("Failed to init FreeType Library");
    }

    FT_Face face;
    if (::FT_New_Memory_Face(ft, bytes.data(), BadgerEngine::numericCast<FT_Long>(bytes.size()).value(), 0, &face)) {
        return unexpected("Failed to load FreeType face");
    }

    return Typeface(std::make_shared<TypefaceImpl>(ft, face));
#else
    return unexpected("Text not supported. Build BadgerEngine with -DBADGER_ENGINE_FREETYPE=ON");
#endif
}

Expected<Font> Typeface::font(std::size_t size) const
{
#ifdef BADGER_ENGINE_FREETYPE
    /// Sets sizes here to check for errors. There is another call of ::FT_Set_Pixel_Sizes in `Font` class to actually set size
    const auto error = ::FT_Set_Pixel_Sizes(m_impl->face(), 0, BadgerEngine::numericCast<FT_UInt>(size).value());
    if (error != 0) {
        return unexpected("Failed to set pixel sizes");
    }

    return Font(m_impl, size);
#else
    return unexpected("Text not supported. Build BadgerEngine with -DBADGER_ENGINE_FREETYPE=ON");
#endif
}

}
