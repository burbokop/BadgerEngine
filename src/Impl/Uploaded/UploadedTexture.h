#pragma once

#include "../Model/Texture.h"
#include "../Utils/Error.h"
#include "../Utils/NoNull.h"
#include <glm/glm.hpp>
#include <map>
#include <vulkan/vulkan.hpp>

namespace BadgerEngine {

class UploadedTextureCache;

class UploadedTexture {
    friend UploadedTextureCache;

public:
    using Size = glm::vec<2, std::size_t, glm::defaultp>;

    static Expected<UploadedTexture> upload(
        const vk::Device& logicalDevice,
        const vk::PhysicalDevice& physicalDevice,
        const vk::CommandPool& commandPool,
        const vk::Queue& copyQueue,
        UploadedTextureCache& cache,
        SharedTexture texture) noexcept;

    static Expected<UploadedTexture> create(
        const vk::Device& logicalDevice,
        const vk::PhysicalDevice& physicalDevice,
        const vk::CommandPool& commandPool,
        const vk::Queue& copyQueue,
        BadgerEngine::UploadedTextureCache& cache,
        PixFormat format,
        Size size,
        RGBAColor fillColor) noexcept;

    const vk::Image& image() const;
    const vk::ImageView& view() const;
    const vk::Format& format() const;
    const Size& size() const;

private:
    struct Impl;

private:
    UploadedTexture(Shared<Impl>&& impl)
        : m_impl(std::move(impl))
    {
    }

private:
    Shared<Impl> m_impl;
};

class UploadedTextureCache {
    friend UploadedTexture;

public:
    UploadedTextureCache() = default;

    UploadedTextureCache& operator=(UploadedTextureCache&&) = delete;
    UploadedTextureCache& operator=(const UploadedTextureCache&) = delete;
    UploadedTextureCache(UploadedTextureCache&&) = delete;
    UploadedTextureCache(const UploadedTextureCache&) = delete;

public:
    struct CreateKey {
        PixFormat format;
        UploadedTexture::Size size;
        RGBAColor fillColor;

        std::partial_ordering operator<=>(const CreateKey& rhs) const
        {
            struct Impl {
                PixFormat f;
                std::size_t w, h;
                float r, g, b, a;

                std::partial_ordering operator<=>(const Impl&) const = default;
            };

            return Impl { format, size.x, size.y, fillColor.r, fillColor.g, fillColor.b, fillColor.a }
            <=> Impl { rhs.format, rhs.size.x, rhs.size.y, rhs.fillColor.r, rhs.fillColor.g, rhs.fillColor.b, rhs.fillColor.a };
        }
    };

private:
    struct TextureCmpLess {
        bool operator()(const std::weak_ptr<Texture>& a, const std::weak_ptr<Texture>& b) const
        {
            return a.lock() < b.lock();
        }
    };

private:
    std::map<std::weak_ptr<Texture>, std::weak_ptr<UploadedTexture::Impl>, TextureCmpLess> m_loadedCache;
    std::map<CreateKey, std::weak_ptr<UploadedTexture::Impl>> m_createdCache;
};

}
