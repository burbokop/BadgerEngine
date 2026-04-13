#include "SwapChain.h"

#include "../Utils/Collections.h"
#include "../Utils/NumericCast.h"
#include <cstdint>
#include <limits>
#include <ranges>
#include <vector>

namespace BadgerEngine {

namespace {

struct ImageInputChunk {
    vk::Device logicalDevice;
    vk::PhysicalDevice physicalDevice;
    std::uint32_t width, height;
    vk::ImageTiling tiling;
    vk::ImageUsageFlags usage;
    vk::MemoryPropertyFlags memoryProperties;
    vk::Format format;
};

vk::SurfaceFormatKHR chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

vk::PresentModeKHR choosePresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Format find_supported_depth_format(vk::PhysicalDevice physicalDevice, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{

    for (vk::Format format : candidates) {

        vk::FormatProperties properties = physicalDevice.getFormatProperties(format);

        /*
        typedef struct VkFormatProperties {
                VkFormatFeatureFlags    linearTilingFeatures;
                VkFormatFeatureFlags    optimalTilingFeatures;
                VkFormatFeatureFlags    bufferFeatures;
        } VkFormatProperties;
        */

        if (tiling == vk::ImageTiling::eLinear
            && (properties.linearTilingFeatures & features) == features) {

            assert(format != vk::Format::eUndefined);
            return format;
        }

        if (tiling == vk::ImageTiling::eOptimal
            && (properties.optimalTilingFeatures & features) == features) {

            assert(format != vk::Format::eUndefined);
            return format;
        }
    }
    throw std::runtime_error("Unable to find suitable format");
}

std::uint32_t findMemoryTypeIndex(vk::PhysicalDevice physicalDevice, std::uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties)
{

    /*
    * // Provided by VK_VERSION_1_0
    typedef struct VkPhysicalDeviceMemoryProperties {
            uint32_t        memoryTypeCount;
            VkMemoryType    memoryTypes[VK_MAX_MEMORY_TYPES];
            uint32_t        memoryHeapCount;
            VkMemoryHeap    memoryHeaps[VK_MAX_MEMORY_HEAPS];
    } VkPhysicalDeviceMemoryProperties;
    */
    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {

        // bit i of supportedMemoryIndices is set if that memory type is supported by the device
        bool supported { static_cast<bool>(supportedMemoryIndices & (1 << i)) };

        // propertyFlags holds all the memory properties supported by this memory type
        bool sufficient { (memoryProperties.memoryTypes[i].propertyFlags & requestedProperties) == requestedProperties };

        if (supported && sufficient) {
            return i;
        }
    }

    return 0;
}

vk::Image make_image(ImageInputChunk input)
{
    const vk::ImageCreateInfo imageInfo = {
        .flags = vk::ImageCreateFlagBits(),
        .imageType = vk::ImageType::e2D,
        .format = input.format,
        .extent = {
            .width = input.width,
            .height = input.height,
            .depth = 1,
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = vk::SampleCountFlagBits::e1,
        .tiling = input.tiling,
        .usage = input.usage,
        .sharingMode = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    try {
        return input.logicalDevice.createImage(imageInfo);
    } catch (const vk::SystemError& err) {
        throw std::runtime_error("Unable to make image: " + std::string(err.what()));
    }
}

vk::DeviceMemory make_image_memory(ImageInputChunk input, vk::Image image)
{
    vk::MemoryRequirements requirements = input.logicalDevice.getImageMemoryRequirements(image);

    vk::MemoryAllocateInfo allocation;
    allocation.allocationSize = requirements.size;
    allocation.memoryTypeIndex = findMemoryTypeIndex(
        input.physicalDevice, requirements.memoryTypeBits, input.memoryProperties);

    try {
        vk::DeviceMemory imageMemory = input.logicalDevice.allocateMemory(allocation);
        input.logicalDevice.bindImageMemory(image, imageMemory, 0);
        return imageMemory;
    } catch (const vk::SystemError& err) {
        throw std::runtime_error("Unable to allocate memory for image: " + std::string(err.what()));
    }
}

vk::ImageView make_image_view(vk::Device logicalDevice, vk::Image image, vk::Format format, vk::ImageAspectFlags aspect)
{

    /*
    * ImageViewCreateInfo( VULKAN_HPP_NAMESPACE::ImageViewCreateFlags flags_ = {},
            VULKAN_HPP_NAMESPACE::Image                image_ = {},
            VULKAN_HPP_NAMESPACE::ImageViewType    viewType_  = VULKAN_HPP_NAMESPACE::ImageViewType::e1D,
            VULKAN_HPP_NAMESPACE::Format           format_    = VULKAN_HPP_NAMESPACE::Format::eUndefined,
            VULKAN_HPP_NAMESPACE::ComponentMapping components_            = {},
            VULKAN_HPP_NAMESPACE::ImageSubresourceRange subresourceRange_ = {} ) VULKAN_HPP_NOEXCEPT
    */

    vk::ImageViewCreateInfo createInfo = {};
    createInfo.image = image;
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = format;
    createInfo.components.r = vk::ComponentSwizzle::eIdentity;
    createInfo.components.g = vk::ComponentSwizzle::eIdentity;
    createInfo.components.b = vk::ComponentSwizzle::eIdentity;
    createInfo.components.a = vk::ComponentSwizzle::eIdentity;
    createInfo.subresourceRange.aspectMask = aspect;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    return logicalDevice.createImageView(createInfo);
}

vk::Extent2D chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const vk::Extent2D& defaultExtent)
{
    if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        vk::Extent2D actualExtent = defaultExtent;

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

Expected<Frame> createFrame(
    const vk::Device& logicalDevice,
    const vk::PhysicalDevice& physicalDevice,
    const vk::RenderPass& colorRenderPass,
    const vk::RenderPass& shadowMapRenderPass,
    const SwapChain::Settings& settings,
    const vk::Image& image) noexcept
{
    assert(image);

    auto colorFrame = [colorRenderPass, settings, logicalDevice, physicalDevice, image] {
        const auto imageView = BadgerEngine::createImageView(logicalDevice, image, settings.surfaceFormat.format);

        const ImageInputChunk depthBufferImageInfo {
            .logicalDevice = logicalDevice,
            .physicalDevice = physicalDevice,
            .width = settings.extent.width,
            .height = settings.extent.height,
            .tiling = vk::ImageTiling::eOptimal,
            .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
            .memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
            .format = settings.depthFormat,
        };

        const auto depthBuffer = make_image(depthBufferImageInfo);
        const auto depthBufferMemory = make_image_memory(depthBufferImageInfo, depthBuffer);
        const auto depthBufferView = make_image_view(
            logicalDevice,
            depthBuffer,
            settings.depthFormat,
            vk::ImageAspectFlagBits::eDepth);

        vk::Framebuffer colorFrameBuffer;
        {
            std::array colorFramebufferAttachments = {
                *imageView,
                depthBufferView
            };

            const vk::FramebufferCreateInfo colorFramebufferInfo = {
                .renderPass = colorRenderPass,
                .attachmentCount = numericCast<std::uint32_t>(colorFramebufferAttachments.size()).value(),
                .pAttachments = colorFramebufferAttachments.data(),
                .width = settings.extent.width,
                .height = settings.extent.height,
                .layers = 1,
            };

            const auto code = logicalDevice.createFramebuffer(&colorFramebufferInfo, nullptr, &colorFrameBuffer);
            if (code != vk::Result::eSuccess) {
                throw std::runtime_error("[error] Failed to create framebuffer: " + vk::to_string(code));
            }
        }

        assert(depthBuffer);
        assert(depthBufferMemory);
        assert(depthBufferView);
        assert(colorFrameBuffer);

        return BadgerEngine::ColorRenderPassFrame {
            .image = image,
            .imageView = *std::move(imageView),
            .depthBuffer = depthBuffer,
            .depthBufferMemory = depthBufferMemory,
            .depthBufferView = depthBufferView,
            .framebuffer = colorFrameBuffer,
        };
    }();

    auto shadowMapFrame = [shadowMapRenderPass, settings, logicalDevice, physicalDevice] {
        const ImageInputChunk shadowMapImageInfo {
            .logicalDevice = logicalDevice,
            .physicalDevice = physicalDevice,
            .width = settings.shadowMapExtent.width,
            .height = settings.shadowMapExtent.height,
            .tiling = vk::ImageTiling::eOptimal,
            .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
            .memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
            .format = settings.depthFormat,
        };

        const auto shadowMap = make_image(shadowMapImageInfo);
        const auto shadowMapMemory = make_image_memory(shadowMapImageInfo, shadowMap);
        const auto shadowMapView = make_image_view(
            logicalDevice,
            shadowMap,
            settings.depthFormat,
            vk::ImageAspectFlagBits::eDepth);

        vk::Framebuffer shadowMapFrameBuffer;
        {
            std::array shadowMapFramebufferAttachments = {
                shadowMapView
            };

            const vk::FramebufferCreateInfo shadowMapFramebufferInfo = {
                .renderPass = shadowMapRenderPass,
                .attachmentCount = numericCast<std::uint32_t>(shadowMapFramebufferAttachments.size()).value(),
                .pAttachments = shadowMapFramebufferAttachments.data(),
                .width = settings.shadowMapExtent.width,
                .height = settings.shadowMapExtent.height,
                .layers = 1,
            };

            const auto code = logicalDevice.createFramebuffer(&shadowMapFramebufferInfo, nullptr, &shadowMapFrameBuffer);
            if (code != vk::Result::eSuccess) {
                throw std::runtime_error("[error] Failed to create framebuffer: " + vk::to_string(code));
            }
        }

        assert(shadowMap);
        assert(shadowMapMemory);
        assert(shadowMapView);
        assert(shadowMapFrameBuffer);

        return BadgerEngine::ShadowMapRenderPassFrame {
            .image = shadowMap,
            .memory = shadowMapMemory,
            .imageView = shadowMapView,
            .framebuffer = shadowMapFrameBuffer,
        };
    }();

    return BadgerEngine::Frame {
        .color = std::move(colorFrame),
        .shadowMap = std::move(shadowMapFrame),
    };
}

Expected<std::vector<Frame>> createSwapchainFrames(
    const vk::Device& logicalDevice,
    const vk::PhysicalDevice& physicalDevice,
    const vk::RenderPass& colorRenderPass,
    const vk::RenderPass& shadowMapRenderPass,
    const SwapChain::Settings& settings,
    const vk::SwapchainKHR& swapchain) noexcept
{
    const auto images = logicalDevice.getSwapchainImagesKHR(swapchain);
    std::vector<Frame> result;
    result.reserve(images.size());
    for (const auto& image : images) {
        const auto frame = createFrame(logicalDevice, physicalDevice, colorRenderPass, shadowMapRenderPass, settings, image);
        if (!frame) {
            return unexpected("Failed to create frame", frame.error());
        }

        result.push_back(*frame);
    }

    return result;
}

}

Expected<vk::ImageView> createImageView(const vk::Device& logicalDevice, vk::Image image, vk::Format format) noexcept
{
    vk::ImageViewCreateInfo viewInfo;
    viewInfo.image = image;
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    vk::ImageView imageView;
    const auto result = logicalDevice.createImageView(&viewInfo, nullptr, &imageView);
    if (result != vk::Result::eSuccess) {
        return unexpected("Failed to create image view: " + vk::to_string(result));
    }

    return imageView;
}

std::vector<std::string> SwapChain::pullErrors()
{
    const auto r = m_errors;
    m_errors.clear();
    return r;
}

bool SwapChain::isValid() const
{
    return m_isValid;
}

vk::Image SwapChain::image(std::size_t index) const
{
    if (index < m_frames.size())
        return m_frames[index].color.image;
    return vk::Image();
}

std::size_t SwapChain::imageCount() const
{
    return m_frames.size();
}

vk::ImageView SwapChain::imageView(std::size_t index) const
{
    if (index < m_frames.size())
        return m_frames[index].color.imageView;
    return vk::ImageView();
}

std::size_t SwapChain::imageViewCount() const
{
    return m_frames.size();
}

SwapChain::Settings SwapChain::settings() const
{
    return m_settings;
}

SwapChain::Settings SwapChain::createSettings(vk::PhysicalDevice physicalDevice, const e172vp::Hardware::SwapChainSupportDetails& supportDetails, const vk::Extent2D& defaultExtent)
{
    return {
        .surfaceFormat = chooseSurfaceFormat(supportDetails.formats),
        .depthFormat = find_supported_depth_format(
            physicalDevice,
            { vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint },
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment),

        .presentMode = choosePresentMode(supportDetails.presentModes),
        .extent = chooseExtent(supportDetails.capabilities, defaultExtent),
        .shadowMapExtent = { 4096, 4096 },
        .supportDetails = supportDetails,
    };
}

std::vector<vk::ImageView> SwapChain::colorImageViewVector() const
{
    return m_frames | std::views::transform([](const BadgerEngine::Frame& f) { return f.color.imageView; }) | BadgerEngine::Collect<std::vector>;
}

std::vector<vk::ImageView> SwapChain::shadowMapImageViewVector() const
{
    return m_frames | std::views::transform([](const BadgerEngine::Frame& f) { return f.shadowMap.imageView; }) | BadgerEngine::Collect<std::vector>;
}

std::vector<vk::Framebuffer> SwapChain::frameBufferVector() const
{
    return m_frames | std::views::transform([](const BadgerEngine::Frame& f) { return f.color.framebuffer; }) | BadgerEngine::Collect<std::vector>;
}

std::vector<vk::Framebuffer> SwapChain::shadowMapFrameBufferVector() const
{
    return m_frames | std::views::transform([](const BadgerEngine::Frame& f) { return f.shadowMap.framebuffer; }) | BadgerEngine::Collect<std::vector>;
}

SwapChain::SwapChain(
    const vk::Device& logicalDevice,
    const vk::PhysicalDevice& physicalDevice,
    const vk::SurfaceKHR& surface,
    const vk::RenderPass& colorRenderPass,
    const vk::RenderPass& shadowMapRenderPass,
    const e172vp::Hardware::QueueFamilies& queueFamilies,
    const Settings& settings)
    : m_logicalDevice(logicalDevice)
{
    uint32_t imageCount = 3;
    if (imageCount > settings.supportDetails.capabilities.maxImageCount) {
        imageCount = settings.supportDetails.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = settings.surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = settings.surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = settings.extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment; // uncommed if u want RecursiveMaterial to work: | vk::ImageUsageFlagBits::eSampled;
    swapchainCreateInfo.setPNext(nullptr);

    uint32_t queueFamilyIndices[] = { queueFamilies.graphicsFamily(), queueFamilies.presentFamily() };

    if (queueFamilies.graphicsFamily() != queueFamilies.presentFamily()) {
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = {};
    }

    swapchainCreateInfo.preTransform = settings.supportDetails.capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapchainCreateInfo.presentMode = settings.presentMode;
    swapchainCreateInfo.clipped = true;

    m_swapChainHandle = logicalDevice.createSwapchainKHR(swapchainCreateInfo);
    m_isValid = m_swapChainHandle != nullptr;
    if (m_isValid) {
        m_settings = settings;

        m_frames = BadgerEngine::createSwapchainFrames(logicalDevice, physicalDevice, colorRenderPass, shadowMapRenderPass, settings, m_swapChainHandle).transform_error(BadgerEngine::AsCritical()).value();

        assert(!m_frames.empty());
    }

    assert(!m_frames.empty());
}

SwapChain::~SwapChain()
{
    for (const auto& frame : m_frames) {
        m_logicalDevice.destroyImage(frame.color.image);
        m_logicalDevice.destroyImageView(frame.color.imageView);
        m_logicalDevice.destroyImage(frame.color.depthBuffer);
        m_logicalDevice.freeMemory(frame.color.depthBufferMemory);
        m_logicalDevice.destroyImageView(frame.color.depthBufferView);
        m_logicalDevice.destroyFramebuffer(frame.color.framebuffer);
        m_logicalDevice.destroyImage(frame.shadowMap.image);
        m_logicalDevice.freeMemory(frame.shadowMap.memory);
        m_logicalDevice.destroyImageView(frame.shadowMap.imageView);
        m_logicalDevice.destroyFramebuffer(frame.shadowMap.framebuffer);
    }
}

}
