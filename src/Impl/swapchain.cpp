#include "swapchain.h"

#include "../Utils/Collections.h"
#include <cstdint>
#include <limits>
#include <ranges>

std::vector<std::string> e172vp::SwapChain::pullErrors()
{
    const auto r = m_errors;
    m_errors.clear();
    return r;
}

bool e172vp::SwapChain::isValid() const
{
    return m_isValid;
}

vk::Image e172vp::SwapChain::image(size_t index) const
{
    if (index < m_frames.size())
        return m_frames[index].color.image;
    return vk::Image();
}

size_t e172vp::SwapChain::imageCount() const
{
    return m_frames.size();
}

vk::ImageView e172vp::SwapChain::imageView(size_t index) const
{
    if (index < m_frames.size())
        return m_frames[index].color.imageView;
    return vk::ImageView();
}

size_t e172vp::SwapChain::imageViewCount() const
{
    return m_frames.size();
}

e172vp::SwapChain::Settings e172vp::SwapChain::settings() const
{
    return m_settings;
}

vk::Image e172vp::SwapChain::make_image(ImageInputChunk input)
{

    /*
    typedef struct VkImageCreateInfo {
            VkStructureType          sType;
            const void* pNext;
            VkImageCreateFlags       flags;
            VkImageType              imageType;
            VkFormat                 format;
            VkExtent3D               extent;
            uint32_t                 mipLevels;
            uint32_t                 arrayLayers;
            VkSampleCountFlagBits    samples;
            VkImageTiling            tiling;
            VkImageUsageFlags        usage;
            VkSharingMode            sharingMode;
            uint32_t                 queueFamilyIndexCount;
            const uint32_t* pQueueFamilyIndices;
            VkImageLayout            initialLayout;
    } VkImageCreateInfo;
    */

    vk::ImageCreateInfo imageInfo;
    imageInfo.flags = vk::ImageCreateFlagBits();
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent = vk::Extent3D(input.width, input.height, 1);
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = input.format;
    imageInfo.tiling = input.tiling;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = input.usage;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.samples = vk::SampleCountFlagBits::e1;

    try {
        return input.logicalDevice.createImage(imageInfo);
    } catch (const vk::SystemError& err) {
        throw std::runtime_error("Unable to make image: " + std::string(err.what()));
    }
}

vk::DeviceMemory e172vp::SwapChain::make_image_memory(ImageInputChunk input, vk::Image image)
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

vk::ImageView e172vp::SwapChain::make_image_view(vk::Device logicalDevice, vk::Image image, vk::Format format, vk::ImageAspectFlags aspect)
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

uint32_t e172vp::SwapChain::findMemoryTypeIndex(vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties)
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

vk::SurfaceFormatKHR e172vp::SwapChain::chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

vk::PresentModeKHR e172vp::SwapChain::choosePresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D e172vp::SwapChain::chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const vk::Extent2D& defaultExtent)
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

e172vp::SwapChain::Settings e172vp::SwapChain::createSettings(vk::PhysicalDevice physicalDevice, const e172vp::Hardware::SwapChainSupportDetails& supportDetails, const vk::Extent2D& defaultExtent)
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
        .shadowMapExtent = { 512, 512 },
        .supportDetails = supportDetails,
    };
}

vk::Format e172vp::SwapChain::find_supported_depth_format(vk::PhysicalDevice physicalDevice, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
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

bool e172vp::SwapChain::createImageViewes(const vk::Device& logicDevice, const std::vector<vk::Image>& swapChainImages, const vk::Format& swapChainImageFormat, std::vector<vk::ImageView>* swapChainImageViews, std::vector<std::string>* error_queue)
{
    swapChainImageViews->resize(swapChainImages.size());

    for (uint32_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews->operator[](i) = createImageView(logicDevice, swapChainImages[i], swapChainImageFormat, error_queue);
        if (!swapChainImageViews->operator[](i))
            return false;
    }
    return true;
}

vk::ImageView e172vp::SwapChain::createImageView(const vk::Device& logicalDevice, vk::Image image, vk::Format format, std::vector<std::string>* error_queue)
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
    if (logicalDevice.createImageView(&viewInfo, nullptr, &imageView) != vk::Result::eSuccess) {
        if (error_queue)
            error_queue->push_back("failed to create texture image view");
    }

    return imageView;
}

std::vector<vk::ImageView> e172vp::SwapChain::colorImageViewVector() const
{
    return m_frames | std::views::transform([](const BadgerEngine::Frame& f) { return f.color.imageView; }) | BadgerEngine::Collect<std::vector>;
}

std::vector<vk::ImageView> e172vp::SwapChain::shadowMapImageViewVector() const
{
    return m_frames | std::views::transform([](const BadgerEngine::Frame& f) { return f.shadowMap.imageView; }) | BadgerEngine::Collect<std::vector>;
}

std::vector<vk::Framebuffer> e172vp::SwapChain::frameBufferVector() const
{
    return m_frames | std::views::transform([](const BadgerEngine::Frame& f) { return f.color.framebuffer; }) | BadgerEngine::Collect<std::vector>;
}

std::vector<vk::Framebuffer> e172vp::SwapChain::shadowMapFrameBufferVector() const
{
    return m_frames | std::views::transform([](const BadgerEngine::Frame& f) { return f.shadowMap.framebuffer; }) | BadgerEngine::Collect<std::vector>;
}

e172vp::SwapChain::SwapChain(
    const vk::Device& logicalDevice,
    const vk::PhysicalDevice& physicalDevice,
    const vk::SurfaceKHR& surface,
    const vk::RenderPass& colorRenderPass,
    const vk::RenderPass& shadowMapRenderPass,
    const Hardware::QueueFamilies& queueFamilies,
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
    swapchainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
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
    m_isValid = m_swapChainHandle;
    if (m_isValid) {
        std::vector<vk::Image> images = logicalDevice.getSwapchainImagesKHR(m_swapChainHandle);
        std::vector<vk::ImageView> imageViewes;
        m_isValid = createImageViewes(logicalDevice, images, settings.surfaceFormat.format, &imageViewes, &m_errors);
        if (m_isValid) {
            m_settings = settings;

            assert(!images.empty());

            assert(images.size() == imageViewes.size());

            m_frames = std::views::zip(images, imageViewes) | std::views::transform([colorRenderPass, shadowMapRenderPass, settings, logicalDevice, physicalDevice](auto&& x) {
                auto [image, imageView] = std::move(x);
                assert(image);
                assert(imageView);

                auto colorFrame = [colorRenderPass, settings, logicalDevice, physicalDevice, image, imageView] -> BadgerEngine::ColorRenderPassFrame {
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
                            imageView,
                            depthBufferView
                        };

                        const vk::FramebufferCreateInfo colorFramebufferInfo = {
                            .renderPass = colorRenderPass,
                            .attachmentCount = colorFramebufferAttachments.size(),
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

                    return {
                        .image = image,
                        .imageView = imageView,
                        .depthBuffer = depthBuffer,
                        .depthBufferMemory = depthBufferMemory,
                        .depthBufferView = depthBufferView,
                        .framebuffer = colorFrameBuffer,
                    };
                }();

                auto shadowMapFrame = [shadowMapRenderPass, settings, logicalDevice, physicalDevice] -> BadgerEngine::ShadowMapRenderPassFrame {
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
                            .attachmentCount = shadowMapFramebufferAttachments.size(),
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

                    return {
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
            }) | BadgerEngine::Collect<std::vector>;

            assert(!m_frames.empty());
        }
    }

    assert(!m_frames.empty());
}

e172vp::SwapChain::~SwapChain()
{
    // TODO
    // for (const auto& frame : m_frames) {
    //     m_logicalDevice.destroyImage(frame.image);
    //     m_logicalDevice.destroyImageView(frame.imageView);
    //     m_logicalDevice.destroyFramebuffer(frame.framebuffer);
    //     m_logicalDevice.destroyImage(frame.depthBuffer);
    //     m_logicalDevice.freeMemory(frame.depthBufferMemory);
    //     m_logicalDevice.destroyImageView(frame.depthBufferView);
    // }
}
