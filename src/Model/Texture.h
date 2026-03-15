#pragma once

#include "../Utils/NoNull.h"
#include <assert.h>
#include <cstdint>
#include <glm/glm.hpp>
#include <optional>
#include <vector>

namespace BadgerEngine {

using Color = glm::vec4;

enum class PixFormat {
    /// Gray scale 8 bit
    GS,
    /// ARGB: alpha mask: 0xff000000, red mask: 0x00ff0000, green mask: 0x0000ff00, blue mask: 0x000000ff
    ARGB32,
    /// RGBA: alpha mask: 0x000000ff, red mask: 0xff000000, green mask: 0x00ff0000, blue mask: 0x0000ff00
    RGBA32
};

inline std::ostream& operator<<(std::ostream& stream, const PixFormat& fmt)
{
    switch (fmt) {
    case PixFormat::GS:
        return stream << "GS";
    case PixFormat::ARGB32:
        return stream << "ARGB32";
    case PixFormat::RGBA32:
        return stream << "RGBA32";
    }
    return stream << "Unknown";
}

/**
 * @brief pixFormatDepth
 * @param fmt
 * @return bits per pixel fr given format. 0 means `fmt` not recognized
 */
constexpr std::size_t pixFormatDepth(PixFormat fmt)
{
    switch (fmt) {
    case PixFormat::GS:
        return 8;
    case PixFormat::ARGB32:
        return 32;
    case PixFormat::RGBA32:
        return 32;
    }
    return 0;
}

/**
 * @brief pixFormatBytesPerPixel
 * @param fmt
 * @return bytes per pixel fr given format. 0 means `fmt` not recognized
 */
constexpr std::size_t pixFormatBytesPerPixel(PixFormat fmt)
{
    switch (fmt) {
    case PixFormat::GS:
        return 1;
    case PixFormat::ARGB32:
        return 4;
    case PixFormat::RGBA32:
        return 4;
    }
    return 0;
}

struct TextureMetaData {
    std::size_t width = 0;
    std::size_t height = 0;
    /// len of one horisontal line in bytes
    std::size_t pitch = 0;
    /// bits per pixel
    std::size_t depth = 0;
    PixFormat format = PixFormat::GS;

    constexpr std::uint32_t alphaMask() const
    {
        switch (format) {
        case PixFormat::GS:
            return 0x00000000;
        case PixFormat::ARGB32:
            return 0xff000000;
        case PixFormat::RGBA32:
            return 0x000000ff;
        }
        std::unreachable();
    }

    constexpr std::uint32_t redMask() const
    {
        switch (format) {
        case PixFormat::GS:
            return 0x000000ff;
        case PixFormat::ARGB32:
            return 0x00ff0000;
        case PixFormat::RGBA32:
            return 0xff000000;
        }
        std::unreachable();
    }

    constexpr std::uint32_t greenMask() const
    {
        switch (format) {
        case PixFormat::GS:
            return 0x000000ff;
        case PixFormat::ARGB32:
            return 0x0000ff00;
        case PixFormat::RGBA32:
            return 0x00ff0000;
        }
        std::unreachable();
    }

    constexpr std::uint32_t blueMask() const
    {
        switch (format) {
        case PixFormat::GS:
            return 0x000000ff;
        case PixFormat::ARGB32:
            return 0x000000ff;
        case PixFormat::RGBA32:
            return 0x0000ff00;
        }
        std::unreachable();
    }
};

class Texture {
    template<typename B>
        requires(sizeof(B) == 1)
    friend class TextureRefImpl;

public:
    explicit Texture(
        std::vector<std::uint8_t>&& data,
        const TextureMetaData& meta)
        : m_data(std::move(data))
        , m_meta(meta)
    {
    }

    static Texture uninitialized(std::size_t w, std::size_t h, PixFormat format)
    {
        const auto depth = pixFormatDepth(format);
        assert(depth != 0);
        std::vector<std::uint8_t> data;
        data.resize(w * h * depth / 8);
        return Texture(std::move(data), TextureMetaData { .width = w, .height = h, .pitch = w * depth / 8, .depth = depth, .format = format });
    }

    template<PixFormat format, typename T>
    static Texture filled(std::size_t w, std::size_t h, T pixel)
    {
        constexpr const auto depth = pixFormatDepth(format);
        assert(depth != 0);
        static_assert(sizeof(T) == depth / 8, "T not match with format depth");

        std::vector<std::uint8_t> data;
        data.resize(w * h * depth / 8);
        auto arr = reinterpret_cast<T*>(data.data());
        // TODO optimize
        for (std::size_t c = w * h, i = 0; i < c; ++i) {
            arr[i] = pixel;
        }
        return Texture(std::move(data), TextureMetaData { .width = w, .height = h, .pitch = w * depth / 8, .depth = depth, .format = format });
    }

    constexpr std::size_t width() const
    {
        return m_meta.width;
    }

    constexpr std::size_t height() const
    {
        return m_meta.height;
    }

    constexpr std::size_t pitch() const
    {
        return m_meta.pitch;
    }

    constexpr std::size_t depth() const
    {
        return m_meta.depth;
    }

    constexpr PixFormat format() const
    {
        return m_meta.format;
    }

    constexpr std::uint32_t alphaMask() const
    {
        return m_meta.alphaMask();
    }

    constexpr std::uint32_t redMask() const
    {
        return m_meta.redMask();
    }

    constexpr std::uint32_t greenMask() const
    {
        return m_meta.greenMask();
    }

    constexpr std::uint32_t blueMask() const
    {
        return m_meta.blueMask();
    }

private:
    std::vector<std::uint8_t> m_data;
    TextureMetaData m_meta;
};

using SharedTexture = Shared<Texture>;

/**
 * @brief The TextureRef class
 * Abstraction over all textures and images
 */
template<typename B>
    requires(sizeof(B) == 1)
class TextureRefImpl {
public:
    TextureRefImpl(Texture& texture)
        : TextureRefImpl(texture.m_data.data(), texture.m_meta)
    {
    }

    TextureRefImpl(SharedTexture texture)
        : TextureRefImpl(texture->m_data.data(), texture->m_meta)
    {
    }

    constexpr static TextureRefImpl fromRaw(
        RawPtr<B> data,
        const TextureMetaData& meta)
    {
        return TextureRefImpl(data, meta);
    }

    constexpr static TextureRefImpl fromRaw(
        RawPtr<B> data, std::size_t w, std::size_t h, PixFormat format)
    {
        const auto depth = pixFormatDepth(format);
        assert(depth != 0);
        return fromRaw(data, TextureMetaData {
                                 .width = w,
                                 .height = h,
                                 .pitch = w * depth / 8,
                                 .depth = depth,
                                 .format = format,
                             });
    }

    constexpr std::size_t width() const
    {
        return m_meta.width;
    }

    constexpr std::size_t height() const
    {
        return m_meta.height;
    }

    constexpr std::size_t pitch() const
    {
        return m_meta.pitch;
    }

    constexpr std::size_t depth() const
    {
        return m_meta.depth;
    }

    constexpr PixFormat format() const
    {
        return m_meta.format;
    }

    constexpr std::uint32_t alphaMask() const
    {
        return m_meta.alphaMask();
    }

    constexpr std::uint32_t redMask() const
    {
        return m_meta.redMask();
    }

    constexpr std::uint32_t greenMask() const
    {
        return m_meta.greenMask();
    }

    constexpr std::uint32_t blueMask() const
    {
        return m_meta.blueMask();
    }

    constexpr std::pair<std::size_t, std::size_t> size() const
    {
        return {
            m_meta.width,
            m_meta.height,
        };
    }

    template<typename T>
    constexpr std::optional<RawPtr<T>> pixels()
        requires(!std::is_const_v<B>)
    {
        if (sizeof(T) == m_meta.depth / 8) {
            return RawPtr<T>(reinterpret_cast<T*>(m_data.nullable()));
        } else {
            return std::nullopt;
        }
    }

    template<typename T>
    constexpr std::optional<RawPtr<const T>> pixels() const
    {
        if (sizeof(T) == m_meta.depth / 8) {
            return RawPtr<const T>(reinterpret_cast<const T*>(m_data.nullable()));
        } else {
            return std::nullopt;
        }
    }

    constexpr RawPtr<B> data()
    {
        return m_data;
    }

    constexpr RawPtr<const B> data() const
    {
        return m_data;
    }

    friend std::ostream& operator<<(std::ostream& stream, const TextureRefImpl& ref)
    {
        return stream << "{ w: " << ref.width()
                      << ", h: " << ref.height()
                      << ", pitch: " << ref.pitch()
                      << ", depth: " << ref.depth()
                      << ", fmt: " << ref.format()
                      << " }";
    }

protected:
    constexpr explicit TextureRefImpl(RawPtr<B> data, const TextureMetaData& meta)
        : m_data(data)
        , m_meta(meta)
    {
    }

private:
    RawPtr<B> m_data;
    TextureMetaData m_meta;
};

using TextureRef = TextureRefImpl<const std::uint8_t>;
using MutableTextureRef = TextureRefImpl<std::uint8_t>;

}
