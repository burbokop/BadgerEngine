#include "swapchain.h"

#include "Tools/stringvector.h"
#include "Utils/Collections.h"
#include <cstdint>
#include <iostream>
#include <limits>
#include <ranges>

vk::SwapchainKHR e172vp::SwapChain::swapChainHandle() const {
    return m_swapChainHandle;
}

e172vp::SwapChain::operator vk::SwapchainKHR() const {
    return m_swapChainHandle;
}

std::vector<std::string> e172vp::SwapChain::pullErrors() {
    const auto r = m_errors;
    m_errors.clear();
    return r;
}

bool e172vp::SwapChain::isValid() const {
    return m_isValid;
}

vk::Image e172vp::SwapChain::image(size_t index) const {
    if (index < m_frames.size())
        return m_frames[index].image;
    return vk::Image();
}

size_t e172vp::SwapChain::imageCount() const {
    return m_frames.size();
}

vk::ImageView e172vp::SwapChain::imageView(size_t index) const {
    if (index < m_frames.size())
        return m_frames[index].imageView;
    return vk::ImageView();
}

size_t e172vp::SwapChain::imageViewCount() const {
    return m_frames.size();
}

e172vp::SwapChain::Settings e172vp::SwapChain::settings() const {
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
    } catch (vk::SystemError err) {
        throw std::runtime_error("Unable to make image");
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
    } catch (vk::SystemError err) {
        throw std::runtime_error("Unable to allocate memory for image");
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

vk::SurfaceFormatKHR e172vp::SwapChain::chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

vk::PresentModeKHR e172vp::SwapChain::choosePresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D e172vp::SwapChain::chooseExtent(const vk::SurfaceCapabilitiesKHR &capabilities, const vk::Extent2D &defaultExtent) {
    if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = defaultExtent;

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

e172vp::SwapChain::Settings e172vp::SwapChain::createSettings(vk::PhysicalDevice physicalDevice, const e172vp::Hardware::SwapChainSupportDetails& supportDetails, const vk::Extent2D& defaultExtent)
{
    Settings result;
    result.surfaceFormat = chooseSurfaceFormat(supportDetails.formats);

    result.depthFormat = find_supported_depth_format(
        physicalDevice,
        { vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);

    result.presentMode = choosePresentMode(supportDetails.presentModes);
    result.extent = chooseExtent(supportDetails.capabilities, defaultExtent);
    result.supportDetails = supportDetails;
    return result;
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

bool e172vp::SwapChain::createImageViewes(const vk::Device &logicDevice, const std::vector<vk::Image> &swapChainImages, const vk::Format &swapChainImageFormat, std::vector<vk::ImageView> *swapChainImageViews, std::vector<std::string> *error_queue) {
    swapChainImageViews->resize(swapChainImages.size());

    for (uint32_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews->operator[](i) = createImageView(logicDevice, swapChainImages[i], swapChainImageFormat, error_queue);
        if(!swapChainImageViews->operator[](i))
            return false;
    }
    return true;
}

vk::ImageView e172vp::SwapChain::createImageView(const vk::Device &logicalDevice, vk::Image image, vk::Format format, std::vector<std::string> *error_queue) {
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
        if(error_queue)
            error_queue->push_back("failed to create texture image view");
    }

    return imageView;
}

std::vector<vk::ImageView> e172vp::SwapChain::imageViewVector() const
{
    return m_frames | std::views::transform([](const BadgerEngine::Frame& f) { return f.imageView; }) | BadgerEngine::Collect<std::vector>;
}

std::vector<vk::Framebuffer> e172vp::SwapChain::frameBufferVector() const
{
    return m_frames | std::views::transform([](const BadgerEngine::Frame& f) { return f.framebuffer; }) | BadgerEngine::Collect<std::vector>;
}

e172vp::SwapChain::SwapChain(
    const vk::Device& logicalDevice,
    const vk::PhysicalDevice& physicalDevice,
    const vk::SurfaceKHR& surface,
    const vk::RenderPass& renderPass,
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

    uint32_t queueFamilyIndices[] = { queueFamilies.graphicsFamily(), queueFamilies.presentFamily()};

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
    if(m_isValid) {
        std::vector<vk::Image> images = logicalDevice.getSwapchainImagesKHR(m_swapChainHandle);
        std::vector<vk::ImageView> imageViewes;
        m_isValid = createImageViewes(logicalDevice, images, settings.surfaceFormat.format, &imageViewes, &m_errors);
        if (m_isValid) {
            m_settings = settings;

            assert(images.size() == imageViewes.size());

            m_frames = std::views::zip(images, imageViewes) | std::views::transform([renderPass, settings, logicalDevice, physicalDevice](auto&& x) {
                auto [image, imageView] = std::move(x);

                ImageInputChunk imageInfo;
                imageInfo.logicalDevice = logicalDevice;
                imageInfo.physicalDevice = physicalDevice;
                imageInfo.tiling = vk::ImageTiling::eOptimal;
                imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
                imageInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
                imageInfo.width = settings.extent.width;
                imageInfo.height = settings.extent.height;
                imageInfo.format = settings.depthFormat;
                const auto depthBuffer = make_image(imageInfo);
                const auto depthBufferMemory = make_image_memory(imageInfo, depthBuffer);
                const auto depthBufferView = make_image_view(
                    logicalDevice,
                    depthBuffer,
                    settings.depthFormat,
                    vk::ImageAspectFlagBits::eDepth);

                std::array attachments = {
                    imageView,
                    depthBufferView
                };

                vk::FramebufferCreateInfo framebufferInfo;
                framebufferInfo.renderPass = renderPass;
                framebufferInfo.attachmentCount = attachments.size();
                framebufferInfo.pAttachments = attachments.data();
                framebufferInfo.width = settings.extent.width;
                framebufferInfo.height = settings.extent.height;
                framebufferInfo.layers = 1;

                vk::Framebuffer frameBuffer;
                const auto code = logicalDevice.createFramebuffer(&framebufferInfo, nullptr, &frameBuffer);
                if (code != vk::Result::eSuccess) {
                    throw std::runtime_error("[error] Failed to create framebuffer: " + vk::to_string(code));
                }

                assert(image);
                assert(imageView);
                assert(depthBuffer);
                assert(depthBufferMemory);
                assert(depthBufferView);
                assert(frameBuffer);

                return BadgerEngine::Frame {
                    .image = image,
                    .imageView = imageView,
                    .depthBuffer = depthBuffer,
                    .depthBufferMemory = depthBufferMemory,
                    .depthBufferView = depthBufferView,
                    .framebuffer = frameBuffer,
                };
            }) | BadgerEngine::Collect<std::vector>;
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
