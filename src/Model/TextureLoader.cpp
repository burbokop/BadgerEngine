#include "TextureLoader.h"

#include "../Utils/NumericCast.h"
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
        .pitch = image.get_width(),
        .depth = 32,
        .format = PixFormat::RGBA32,
    };

    std::vector<std::uint8_t> data(metadata.width * metadata.height * metadata.depth / 8);

    for (std::size_t y = 0; y < metadata.height; ++y) {
        const auto in = image.get_row(y);
        std::uint8_t* out = &data[y * metadata.width];
        std::memcpy(out, in.data(), metadata.width);
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
