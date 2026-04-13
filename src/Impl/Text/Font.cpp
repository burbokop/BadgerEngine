#include "Font.h"

#include "../Utils/NumericCast.h"

#ifdef BADGER_ENGINE_FREETYPE
#include "TypefaceImpl.h"
#endif

namespace BadgerEngine {

namespace {

}

Expected<SharedTexture> Font::glyph(char c) const
{
#ifdef BADGER_ENGINE_FREETYPE
    const auto error = ::FT_Set_Pixel_Sizes(m_impl->face(), 0, BadgerEngine::numericCast<FT_UInt>(m_size).value());
    if (error != 0) {
        return unexpected("Failed to set pixel sizes");
    }

    if (c < 0) {
        return unexpected("Only supports ASCII characters from 0 to 127");
    }

    if (::FT_Load_Char(m_impl->face(), static_cast<FT_ULong>(c), FT_LOAD_RENDER)) {
        return unexpected("Failed to load Glyph for char `" + std::string(1, c) + "`");
    }

    const auto glyph = m_impl->face()->glyph;

    if (glyph->bitmap.width == 0) {
        return unexpected("Glyph width is zero");
    }

    if (glyph->bitmap.rows == 0) {
        return unexpected("Glyph rows is zero");
    }

    const auto format = PixFormat::RGBA32;
    const auto bytesPerPixel = pixFormatBytesPerPixel(format);

    const auto metadata = TextureMetaData {
        .width = glyph->bitmap.width,
        .height = glyph->bitmap.rows,
        .pitch = glyph->bitmap.width * bytesPerPixel,
        .depth = pixFormatDepth(format),
        .format = format,
    };

    std::vector<std::uint8_t> data(metadata.pitch * metadata.height);

    const auto p = reinterpret_cast<std::uint8_t*>(glyph->bitmap.buffer);
    for (std::size_t y = 0; y < glyph->bitmap.rows; ++y) {
        for (std::size_t x = 0; x < glyph->bitmap.width; ++x) {
            std::uint8_t in = p[x + y * glyph->bitmap.width];
            std::uint8_t* out = &data.data()[x * bytesPerPixel + y * metadata.pitch];
            out[0] = in;
            out[1] = in;
            out[2] = in;
            out[3] = 0xff;
        }
    }

    return std::make_shared<Texture>(std::move(data), metadata);
#else
    return unexpected("Text not supported. Build BadgerEngine with -DBADGER_ENGINE_FREETYPE=ON");
#endif
}

Expected<SharedTexture> Font::text(const std::string& str) const
{
#ifdef BADGER_ENGINE_FREETYPE
    const auto error = ::FT_Set_Pixel_Sizes(m_impl->face(), 0, BadgerEngine::numericCast<FT_UInt>(m_size).value());
    if (error != 0) {
        return unexpected("Failed to set pixel sizes");
    }

    if (str.empty()) {
        return unexpected("Empty string");
    }

    std::size_t width = 0;
    std::size_t height = 0;
    std::size_t maxTop = 0;

    for (const auto& c : str) {
        if (c < 0) {
            return unexpected("Only supports ASCII characters from 0 to 127");
        }

        if (::FT_Load_Char(m_impl->face(), static_cast<FT_ULong>(c), FT_LOAD_RENDER)) {
            return unexpected("Failed to load Glyph for char `" + std::string(1, c) + "`");
        }

        const auto glyph = m_impl->face()->glyph;

        const auto glyphTop = BadgerEngine::numericCast<std::size_t>(glyph->bitmap_top).value();
        const auto glyphHeight = BadgerEngine::numericCast<std::size_t>(glyph->bitmap.rows).value();
        const auto advanceX = BadgerEngine::numericCast<std::uint32_t>(glyph->advance.x).value();

        width += advanceX >> 6;
        height = std::max(height, glyphHeight);
        maxTop = std::max(maxTop, glyphTop);
    }

    if (width == 0) {
        return unexpected("Total width is 0");
    }

    if (height == 0) {
        return unexpected("Total height is 0");
    }

    const auto format = PixFormat::RGBA32;
    const auto bytesPerPixel = pixFormatBytesPerPixel(format);

    const auto metadata = TextureMetaData {
        .width = width,
        .height = height,
        .pitch = width * bytesPerPixel,
        .depth = pixFormatDepth(format),
        .format = format,
    };

    std::vector<std::uint8_t> data(metadata.pitch * metadata.height);

    std::size_t offsetX = 0;

    for (const auto& c : str) {
        if (c < 0) {
            return unexpected("Only supports ASCII characters from 0 to 127");
        }

        if (::FT_Load_Char(m_impl->face(), static_cast<FT_ULong>(c), FT_LOAD_RENDER)) {
            return unexpected("Failed to load Glyph for char `" + std::string(1, c) + "`");
        }

        const auto glyph = m_impl->face()->glyph;
        const auto glyphTop = BadgerEngine::numericCast<std::size_t>(glyph->bitmap_top).value();
        const auto advanceX = BadgerEngine::numericCast<std::uint32_t>(glyph->advance.x).value();
        const std::size_t offsetY = maxTop - glyphTop;

        const auto p = reinterpret_cast<std::uint8_t*>(glyph->bitmap.buffer);
        for (std::size_t y = 0; y < glyph->bitmap.rows; ++y) {
            for (std::size_t x = 0; x < glyph->bitmap.width; ++x) {
                std::uint8_t in = p[x + y * glyph->bitmap.width];

                const auto dstX = offsetX + x;
                const auto dstY = offsetY + y;
                if (dstX < metadata.width && dstY < metadata.height) {
                    std::uint8_t* out = &data.data()[dstX * bytesPerPixel + dstY * metadata.pitch];
                    out[0] = in;
                    out[1] = in;
                    out[2] = in;
                    out[3] = 0xff;
                }
            }
        }

        offsetX += advanceX >> 6;
    }

    return std::make_shared<Texture>(std::move(data), metadata);

#else
    return unexpected("Text not supported. Build BadgerEngine with -DBADGER_ENGINE_FREETYPE=ON");
#endif
}

}
