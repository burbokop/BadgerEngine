#include "TextureLoader.h"

#include "../Utils/Collections.h"
#include <cstring>
#include <execution>
#include <future>
#include <iostream>
#include <spng.h>

namespace BadgerEngine {

namespace {

std::optional<spng_format> spngFormatFromPixFormat(PixFormat fmt)
{
    switch (fmt) {
    case PixFormat::GS:
        return spng_format::SPNG_FMT_G8;
    case PixFormat::ARGB32:
        return std::nullopt;
    case PixFormat::RGBA32:
        return spng_format::SPNG_FMT_RGBA8;
    }
    std::unreachable();
}

Expected<SharedTexture> parsePNG(std::span<const std::uint8_t> input, PixFormat format) noexcept
{
    spng_ctx* ctx = ::spng_ctx_new(0);
    assert(ctx);

    {
        const auto result = ::spng_set_png_buffer(ctx, input.data(), input.size());
        if (result != SPNG_OK) {
            return unexpected(std::string("spng_set_png_buffer failed: ") + ::spng_strerror(result));
        }
    }

    const auto f = spngFormatFromPixFormat(format);
    if (!f) {
        return unexpected("Unsupported format");
    }

    spng_ihdr ihdr = {};

    {
        const auto result = ::spng_get_ihdr(ctx, &ihdr);
        if (result != SPNG_OK) {
            return unexpected(std::string("spng_get_image_limits failed: ") + ::spng_strerror(result));
        }
    }

    const auto bytesPerPixel = pixFormatBytesPerPixel(format);

    const auto metadata = TextureMetaData {
        .width = ihdr.width,
        .height = ihdr.height,
        .pitch = ihdr.width * bytesPerPixel,
        .depth = pixFormatDepth(format),
        .format = format,
    };

    std::size_t outputByteSize = 0;

    {
        const auto result = ::spng_decoded_image_size(ctx, *f, &outputByteSize);
        if (result != SPNG_OK) {
            return unexpected(std::string("spng_decoded_image_size failed: ") + ::spng_strerror(result));
        }
    }

    std::vector<std::uint8_t> data(outputByteSize);

    {
        const auto result = ::spng_decode_image(ctx, data.data(), outputByteSize, *f, 0);
        if (result != SPNG_OK) {
            return unexpected(std::string("spng_decode_image failed: ") + ::spng_strerror(result));
        }
    }

    ::spng_ctx_free(ctx);

    return std::make_shared<Texture>(std::move(data), metadata);
}

}

Expected<void> BadgerEngine::TextureLoader::load(const BadgerEngine::TextureLoader::VirtualTexturePath& vpath, const std::filesystem::path& path) noexcept
{
    (void)vpath;
    (void)path;
    return unexpected("TODO");
}

Expected<void> TextureLoader::load(std::map<VirtualTexturePath, std::filesystem::path> recipes) noexcept
{
    (void)recipes;
    return unexpected("TODO");
}

Expected<void> TextureLoader::parse(const VirtualTexturePath& vpath, std::span<const std::uint8_t> bytes) noexcept
{
    const auto texture = parsePNG(bytes, PixFormat::RGBA32);
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

Expected<void> TextureLoader::parse(std::map<VirtualTexturePath, std::span<const uint8_t>> recipes) noexcept
{
    struct ResultElement {
        VirtualTexturePath vpath;
        Expected<SharedTexture> texture;
    };

    auto result = std::move(recipes)
        | std::views::transform([](auto&& input) {
              return std::async([](auto&& item) -> ResultElement {
                  return { item.first, parsePNG(item.second, PixFormat::RGBA32) };
              },
                  std::move(input));
          })
        | Collect<std::vector>;

    for (auto&& future : std::move(result)) {
        const auto& [vpath, texture] = future.get();

        if (!texture) {
            return unexpected("Failed to parse image for vpath `" + vpath + "`", texture.error());
        }

        const auto it = m_textures.find(vpath);
        if (it != m_textures.end()) {
            return unexpected("Texture with virtual path `" + vpath + "` already exist");
        }

        m_textures.insert(it, { vpath, *texture });
    }

    return {};
}

}
