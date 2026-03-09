#include "TextureLoader.h"

#include <cstring>
#include <png++/png.hpp>

namespace BadgerEngine {

namespace {

Expected<SharedTexture> readPNG(const std::filesystem::path& path) noexcept
{
    png::image<png::rgba_pixel> image(path);

    const auto metadata = TextureMetaData {
        .width = image.get_width(),
        .height = image.get_height(),
        .pitch = image.get_width() * 4,
        .depth = 32,
        .format = PixFormat::RGBA32,
    };

    std::vector<std::uint8_t> data(metadata.pitch * metadata.height);

    // Produces wrong color. TODO: Figure out why and fix
    // for (std::size_t y = 0; y < metadata.height; ++y) {
    //     const auto in = image.get_row(y);
    //     std::uint8_t* out = &data.data()[y * metadata.pitch];
    //     std::memcpy(out, in.data(), metadata.pitch);
    // }

    for (std::size_t y = 0; y < metadata.height; ++y) {
        for (std::size_t x = 0; x < metadata.width; ++x) {
            const auto in = image.get_pixel(x, y);
            std::uint32_t* out = &reinterpret_cast<std::uint32_t*>(data.data())[x + y * metadata.width];

            *out
                = (static_cast<std::uint32_t>(in.red) & 0x000000ff) << 24
                | (static_cast<std::uint32_t>(in.green) & 0x000000ff) << 16
                | (static_cast<std::uint32_t>(in.blue) & 0x000000ff) << 8
                | (static_cast<std::uint32_t>(in.alpha) & 0x000000ff) << 0;
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

    const auto texture = readPNG(path);
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
