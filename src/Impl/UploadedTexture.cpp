#include "UploadedTexture.h"

#include "../Utils/NumericCast.h"
#include "Buffers/BufferUtils.h"
#include "swapchain.h"

namespace BadgerEngine {

namespace {

[[nodiscard]] Expected<void> createImage(
    const vk::Device& logicalDevice,
    const vk::PhysicalDevice& physicalDevice,
    std::uint32_t width,
    std::uint32_t height,
    vk::Format format,
    vk::ImageTiling tiling,
    vk::ImageUsageFlags usage,
    vk::MemoryPropertyFlags properties,
    vk::Image* image,
    vk::DeviceMemory* imageMemory) noexcept
{
    vk::ImageCreateInfo imageInfo {};
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = usage;
    imageInfo.samples = vk::SampleCountFlagBits::e1;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;

    if (logicalDevice.createImage(&imageInfo, nullptr, image) != vk::Result::eSuccess) {
        return unexpected("failed to create image!");
    }

    vk::MemoryRequirements memRequirements;
    logicalDevice.getImageMemoryRequirements(*image, &memRequirements);

    vk::MemoryAllocateInfo allocInfo {};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = BadgerEngine::BufferUtils::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    const auto code = logicalDevice.allocateMemory(&allocInfo, nullptr, imageMemory);
    if (code != vk::Result::eSuccess) {
        return unexpected("failed to allocate image memory: " + vk::to_string(code));
    }

    logicalDevice.bindImageMemory(*image, *imageMemory, 0);

    return {};
}

[[nodiscard]] Expected<vk::CommandBuffer> beginSingleTimeCommands(const vk::Device& logicalDevice, const vk::CommandPool& commandPool) noexcept
{
    vk::CommandBufferAllocateInfo allocInfo {};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    vk::CommandBuffer commandBuffer;
    {
        const auto result = logicalDevice.allocateCommandBuffers(&allocInfo, &commandBuffer);
        if (result != vk::Result::eSuccess) {
            return unexpected("Failed to allocate command buffers: " + vk::to_string(result));
        }
    }

    vk::CommandBufferBeginInfo beginInfo {};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    {
        const auto result = commandBuffer.begin(&beginInfo);
        if (result != vk::Result::eSuccess) {
            return unexpected("Failed to begin command buffer: " + vk::to_string(result));
        }
    }

    return commandBuffer;
}

[[nodiscard]] Expected<void> endSingleTimeCommands(
    const vk::Device& logicalDevice,
    const vk::CommandPool& commandPool,
    const vk::Queue& queue,
    vk::CommandBuffer commandBuffer) noexcept
{
    commandBuffer.end();

    vk::SubmitInfo submitInfo {};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    const auto result = queue.submit(1, &submitInfo, vk::Fence());
    if (result != vk::Result::eSuccess) {
        return unexpected("Failed to submit queue: " + vk::to_string(result));
    }
    queue.waitIdle();

    logicalDevice.freeCommandBuffers(commandPool, 1, &commandBuffer);

    return {};
}

[[nodiscard]] Expected<void> transitionImageLayout(
    const vk::Device& logicalDevice,
    const vk::CommandPool& commandPool,
    const vk::Queue& queue,
    vk::Image image,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout) noexcept
{
    const auto commandBuffer = beginSingleTimeCommands(logicalDevice, commandPool);
    if (!commandBuffer) {
        return unexpected("Failed to begin single time commands", commandBuffer.error());
    }

    vk::ImageMemoryBarrier barrier;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        return unexpected("Unsupported layout transition");
    }

    commandBuffer->pipelineBarrier(
        sourceStage,
        destinationStage,
        vk::DependencyFlags(),
        0, nullptr,
        0, nullptr,
        1, &barrier);

    const auto result = endSingleTimeCommands(logicalDevice, commandPool, queue, *commandBuffer);
    if (!result) {
        return unexpected("Failed to end single time commands", result.error());
    }

    return {};
}

[[nodiscard]] Expected<void> copyBufferToImage(
    const vk::Device& logicalDevice,
    const vk::CommandPool& commandPool,
    const vk::Queue& queue,
    vk::Buffer buffer,
    vk::Image image,
    std::uint32_t width,
    std::uint32_t height) noexcept
{
    const auto commandBuffer = beginSingleTimeCommands(logicalDevice, commandPool);

    if (!commandBuffer) {
        return unexpected("Failed to begin single time commands", commandBuffer.error());
    }

    vk::BufferImageCopy region {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = vk::Offset3D { 0, 0, 0 };
    region.imageExtent = vk::Extent3D {
        width,
        height,
        1
    };

    commandBuffer->copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

    const auto result = endSingleTimeCommands(logicalDevice, commandPool, queue, *commandBuffer);
    if (!result) {
        return unexpected("Failed to end single time commands", result.error());
    }

    return {};
}

[[nodiscard]] Expected<void> createTextureImage32(
    const vk::Device& logicalDevice,
    const vk::PhysicalDevice& physicalDevice,
    const vk::CommandPool& commandPool,
    const vk::Queue& copyQueue,
    const std::uint8_t* pixels,
    std::size_t w,
    std::size_t h,
    vk::Format format,
    vk::Image* image,
    vk::DeviceMemory* imageMemory) noexcept
{
    std::size_t channelCount = 0;
    if (format == vk::Format::eR8G8B8Srgb) {
        channelCount = 3;
    } else if (format == vk::Format::eR8G8B8A8Srgb || format == vk::Format::eA8B8G8R8SrgbPack32) {
        channelCount = 4;
    } else if (format == vk::Format::eR8Srgb) {
        channelCount = 1;
    } else {
        return unexpected("Unsupported image format");
    }

    const vk::DeviceSize imageSize = w * h * channelCount;

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    if (!BadgerEngine::BufferUtils::createAbstractBuffer(
            logicalDevice,
            physicalDevice,
            imageSize,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            &stagingBuffer,
            &stagingBufferMemory)) {
        return unexpected("Failed to create image buffer");
    }

    {
        void* data;
        const auto result = logicalDevice.mapMemory(stagingBufferMemory, 0, imageSize, vk::MemoryMapFlags(), &data);
        if (result != vk::Result::eSuccess) {
            return unexpected("Failed to map memory: " + vk::to_string(result));
        }

        std::memcpy(data, pixels, static_cast<std::size_t>(imageSize));
        logicalDevice.unmapMemory(stagingBufferMemory);
    }

    {
        const auto result = createImage(
            logicalDevice,
            physicalDevice,
            BadgerEngine::numericCast<std::uint32_t>(w).value(),
            BadgerEngine::numericCast<std::uint32_t>(h).value(),
            format,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            vk::MemoryPropertyFlagBits::eDeviceLocal, image, imageMemory);

        if (!result) {
            return unexpected("Failed to create image", result.error());
        }
    }

    {
        const auto result = transitionImageLayout(logicalDevice, commandPool, copyQueue, *image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        if (!result) {
            return unexpected("Failed to transition image layout", result.error());
        }
    }

    {
        const auto result = copyBufferToImage(logicalDevice, commandPool, copyQueue, stagingBuffer, *image, static_cast<uint32_t>(w), static_cast<uint32_t>(h));
        if (!result) {
            return unexpected("Failed to copy buffer to image", result.error());
        }
    }

    {
        const auto result = transitionImageLayout(logicalDevice, commandPool, copyQueue, *image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
        if (!result) {
            return unexpected("Failed to transition image layout", result.error());
        }
    }

    logicalDevice.destroyBuffer(stagingBuffer);
    logicalDevice.freeMemory(stagingBufferMemory);
    return {};
}

std::optional<vk::Format> pixFormatToVkFormat(PixFormat fmt)
{
    switch (fmt) {
    case PixFormat::GS:
        return std::nullopt;
    case PixFormat::ARGB32:
        return std::nullopt;
    case PixFormat::RGBA32:
        return vk::Format::eR8G8B8A8Srgb;
    }
    std::unreachable();
}

std::vector<std::uint8_t> colorToBytesOfPixFormat(PixFormat fmt, RGBAColor color)
{
    switch (fmt) {
    case PixFormat::GS:
        return { static_cast<std::uint8_t>((color.a + color.r + color.g + color.b) * 255.f / 4.f) };
    case PixFormat::ARGB32:
        return {
            static_cast<std::uint8_t>(color.a * 255.f),
            static_cast<std::uint8_t>(color.r * 255.f),
            static_cast<std::uint8_t>(color.g * 255.f),
            static_cast<std::uint8_t>(color.b * 255.f),
        };
    case PixFormat::RGBA32:
        return {
            static_cast<std::uint8_t>(color.r * 255.f),
            static_cast<std::uint8_t>(color.g * 255.f),
            static_cast<std::uint8_t>(color.b * 255.f),
            static_cast<std::uint8_t>(color.a * 255.f),
        };
    }
    std::unreachable();
}

std::vector<std::uint8_t> textureBytesFilledWithColor(PixFormat fmt, UploadedTexture::Size size, RGBAColor color)
{
    const auto pix = colorToBytesOfPixFormat(fmt, color);

    std::vector<std::uint8_t> result;
    result.reserve(size.x * size.y * pix.size());
    for (std::size_t i = 0; i < size.x * size.y; ++i) {
        result.insert(result.end(), pix.begin(), pix.end());
    }
    return result;
}

}

Expected<Shared<UploadedTexture>> UploadedTexture::upload(
    const vk::Device& logicalDevice,
    const vk::PhysicalDevice& physicalDevice,
    const vk::CommandPool& commandPool,
    const vk::Queue& copyQueue,
    BadgerEngine::UploadedTextureCache& cache,
    SharedTexture texture) noexcept
{
    {
        const auto it = cache.m_loadedCache.find(texture.nullable());
        if (it != cache.m_loadedCache.end()) {
            const auto key = it->first.lock();
            const auto value = it->second.lock();
            if (key && value) {
                return value;
            } else {
                cache.m_loadedCache.erase(it);
            }
        }
    }

    const auto imageFormat = pixFormatToVkFormat(texture->format());
    if (!imageFormat) {
        std::ostringstream ss;
        ss << texture->format();
        return unexpected("Unsupported pixel format: " + ss.str());
    }

    vk::DeviceMemory imageMemory;
    vk::Image image;

    {
        const auto result = createTextureImage32(
            logicalDevice,
            physicalDevice,
            commandPool,
            copyQueue,
            texture->data().nullable(),
            texture->width(),
            texture->height(),
            *imageFormat,
            &image,
            &imageMemory);
        if (!result) {
            return unexpected("Failed to create texture image", result.error());
        }
    }

    std::vector<std::string> errs;
    const auto imageView = e172vp::SwapChain::createImageView(logicalDevice, image, *imageFormat, &errs);
    if (!errs.empty()) {
        return unexpected("Error creating image view: " + errs.front());
    }

    const auto size = glm::ivec2(texture->width(), texture->height());

    const auto result = std::make_shared<UploadedTexture>(
        std::move(logicalDevice),
        std::move(imageMemory),
        std::move(image),
        std::move(imageView),
        *std::move(imageFormat),
        std::move(size),
        Private {});

    [[maybe_unused]] const auto ok = cache.m_loadedCache.insert({ texture.nullable(), result }).second;
    assert(ok);

    return result;
}

Expected<Shared<UploadedTexture>> UploadedTexture::create(
    const vk::Device& logicalDevice,
    const vk::PhysicalDevice& physicalDevice,
    const vk::CommandPool& commandPool,
    const vk::Queue& copyQueue,
    BadgerEngine::UploadedTextureCache& cache,
    PixFormat format,
    Size size,
    RGBAColor fillColor) noexcept
{
    const auto cacheKey = UploadedTextureCache::CreateKey { format, size, fillColor };
    {
        const auto it = cache.m_createdCache.find(cacheKey);
        if (it != cache.m_createdCache.end()) {
            const auto value = it->second.lock();
            if (value) {
                return value;
            } else {
                cache.m_createdCache.erase(it);
            }
        }
    }

    const auto imageFormat = pixFormatToVkFormat(format);
    if (!imageFormat) {
        std::ostringstream ss;
        ss << format;
        return unexpected("Unsupported pixel format: " + ss.str());
    }

    vk::DeviceMemory imageMemory;
    vk::Image image;

    const auto data = textureBytesFilledWithColor(format, size, fillColor);

    {
        const auto result = createTextureImage32(
            logicalDevice,
            physicalDevice,
            commandPool,
            copyQueue,
            data.data(),
            size.x,
            size.y,
            *imageFormat,
            &image,
            &imageMemory);
        if (!result) {
            return unexpected("Failed to create texture image", result.error());
        }
    }

    std::vector<std::string> errs;
    const auto imageView = e172vp::SwapChain::createImageView(logicalDevice, image, *imageFormat, &errs);
    if (!errs.empty()) {
        return unexpected("Error creating image view: " + errs.front());
    }

    const auto result = std::make_shared<UploadedTexture>(
        std::move(logicalDevice),
        std::move(imageMemory),
        std::move(image),
        std::move(imageView),
        *std::move(imageFormat),
        std::move(size),
        Private {});

    [[maybe_unused]] const auto ok = cache.m_createdCache.insert({ cacheKey, result }).second;
    assert(ok);

    return result;
}

UploadedTexture::~UploadedTexture()
{
    if (m_logicalDevice) {
        assert(m_imageView);
        assert(m_image);
        assert(m_imageMemory);
        m_logicalDevice.destroyImageView(m_imageView);
        m_logicalDevice.destroyImage(m_image);
        m_logicalDevice.freeMemory(m_imageMemory);
    }
}

}
