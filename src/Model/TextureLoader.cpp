#include "TextureLoader.h"

#include <cstring>
#include <png++/png.hpp>

namespace BadgerEngine {

namespace {

class MemBuf : public std::streambuf {
public:
    MemBuf(std::span<const std::uint8_t> bytes)
    {
        char* p(const_cast<char*>(reinterpret_cast<const char*>(bytes.data())));
        this->setg(p, p, p + bytes.size());
    }
};

class IMemStream : virtual MemBuf, public std::istream {
public:
    IMemStream(std::span<const std::uint8_t> bytes)
        : MemBuf(bytes)
        , std::istream(static_cast<std::streambuf*>(this))
    {
    }
};

Expected<SharedTexture> parsePNG(const png::image<png::rgba_pixel>& image, PixFormat format) noexcept
{
    const auto bytesPerPixel = pixFormatBytesPerPixel(format);

    const auto metadata = TextureMetaData {
        .width = image.get_width(),
        .height = image.get_height(),
        .pitch = image.get_width() * bytesPerPixel,
        .depth = pixFormatDepth(format),
        .format = format,
    };

    std::vector<std::uint8_t> data(metadata.pitch * metadata.height);
    for (std::size_t y = 0; y < metadata.height; ++y) {
        for (std::size_t x = 0; x < metadata.width; ++x) {
            const auto in = image.get_pixel(x, y);
            std::uint8_t* out = &data.data()[x * bytesPerPixel + y * metadata.pitch];

            switch (format) {
            case PixFormat::GS:
                out[0] = static_cast<std::uint8_t>((static_cast<std::uint16_t>(in.alpha) + static_cast<std::uint16_t>(in.red) + static_cast<std::uint16_t>(in.green) + static_cast<std::uint16_t>(in.blue)) / 4);
                break;
            case PixFormat::ARGB32:
                out[0] = in.alpha;
                out[1] = in.red;
                out[2] = in.green;
                out[3] = in.blue;
                break;
            case PixFormat::RGBA32:
                out[0] = in.red;
                out[1] = in.green;
                out[2] = in.blue;
                out[3] = in.alpha;
                break;
            }
        }
    }

    return std::make_shared<Texture>(std::move(data), metadata);
}

}

Expected<void> BadgerEngine::TextureLoader::load(const BadgerEngine::TextureLoader::VirtualTexturePath& vpath, const std::filesystem::path& path) noexcept
{
    if (!std::filesystem::exists(path)) {
        return unexpected("File `" + path.string() + "` not exist");
    }

    png::image<png::rgba_pixel> image(path);
    const auto texture = parsePNG(image, PixFormat::RGBA32);
    if (!texture) {
        return unexpected(texture.error());
    }

    const auto it = m_textures.find(vpath);
    if (it != m_textures.end()) {
        return unexpected("Texture with virtual path `" + vpath + "` already exist");
    }

    m_textures.insert(it, { vpath, *texture });
    return {};
}

Expected<void> TextureLoader::parse(const VirtualTexturePath& vpath, std::span<const uint8_t> bytes) noexcept
{
    IMemStream stream(bytes);
    png::image<png::rgba_pixel> image(stream);
    const auto texture = parsePNG(image, PixFormat::RGBA32);
    if (!texture) {
        return unexpected(texture.error());
    }

    const auto it = m_textures.find(vpath);
    if (it != m_textures.end()) {
        return unexpected("Texture with virtual path `" + vpath + "` already exist");
    }

    m_textures.insert(it, { vpath, *texture });
    return {};
}

}
