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
    struct Private {};

public:
    using Size = glm::vec<2, std::size_t, glm::defaultp>;

    UploadedTexture& operator=(UploadedTexture&&) = delete;
    UploadedTexture& operator=(const UploadedTexture&) = delete;
    UploadedTexture(UploadedTexture&&) = delete;
    UploadedTexture(const UploadedTexture&) = delete;

    static Expected<Shared<UploadedTexture>> upload(
        const vk::Device& logicalDevice,
        const vk::PhysicalDevice& physicalDevice,
        const vk::CommandPool& commandPool,
        const vk::Queue& copyQueue,
        UploadedTextureCache& cache,
        SharedTexture texture) noexcept;

    static Expected<Shared<UploadedTexture>> create(
        const vk::Device& logicalDevice,
        const vk::PhysicalDevice& physicalDevice,
        const vk::CommandPool& commandPool,
        const vk::Queue& copyQueue,
        BadgerEngine::UploadedTextureCache& cache,
        PixFormat format,
        Size size,
        Color fillColor) noexcept;

    UploadedTexture(
        vk::Device logicalDevice,
        vk::DeviceMemory imageMemory,
        vk::Image image,
        vk::ImageView imageView,
        vk::Format imageFormat,
        Size size,
        Private)
        : m_logicalDevice(std::move(logicalDevice))
        , m_imageMemory(std::move(imageMemory))
        , m_image(std::move(image))
        , m_imageView(std::move(imageView))
        , m_imageFormat(std::move(imageFormat))
        , m_size(std::move(size))
    {
    }

    ~UploadedTexture();

    vk::Image image() const { return m_image; }
    vk::ImageView imageView() const { return m_imageView; }
    vk::Format imageFormat() const { return m_imageFormat; }
    Size size() const { return m_size; }

private:
    vk::Device m_logicalDevice;
    vk::DeviceMemory m_imageMemory;
    vk::Image m_image;
    vk::ImageView m_imageView;
    vk::Format m_imageFormat;
    Size m_size;
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
        Color fillColor;

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
    std::map<std::weak_ptr<Texture>, std::weak_ptr<UploadedTexture>, TextureCmpLess> m_loadedCache;
    std::map<CreateKey, std::weak_ptr<UploadedTexture>> m_createdCache;
};

}
