#pragma once

#include "../Model/Texture.h"
#include "../Utils/Error.h"
#include "../Utils/NoNull.h"
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace BadgerEngine {

class UploadedTexture {
    struct Private {};

public:
    UploadedTexture& operator=(UploadedTexture&&) = delete;
    UploadedTexture& operator=(const UploadedTexture&) = delete;
    UploadedTexture(UploadedTexture&&) = delete;
    UploadedTexture(const UploadedTexture&) = delete;

    static Expected<Shared<UploadedTexture>> upload(
        const vk::Device& logicalDevice,
        const vk::PhysicalDevice& physicalDevice,
        const vk::CommandPool& commandPool,
        const vk::Queue& copyQueue,
        BitmapRef texture) noexcept;

    UploadedTexture(
        vk::Device logicalDevice,
        vk::DeviceMemory imageMemory,
        vk::Image image,
        vk::ImageView imageView,
        vk::Format imageFormat,
        glm::ivec2 size,
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
    glm::ivec2 size() const { return m_size; }

private:
    vk::Device m_logicalDevice;
    vk::DeviceMemory m_imageMemory;
    vk::Image m_image;
    vk::ImageView m_imageView;
    vk::Format m_imageFormat;
    glm::ivec2 m_size;
};
}
