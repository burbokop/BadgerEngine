#pragma once

#include "Tools/hardware.h"
#include <vulkan/vulkan.hpp>

namespace BadgerEngine {

struct Frame {
    vk::Image image;
    vk::ImageView imageView;

    vk::Image depthBuffer;
    vk::DeviceMemory depthBufferMemory;
    vk::ImageView depthBufferView;

    vk::Framebuffer framebuffer;
};

}

namespace e172vp {

class SwapChain {
    vk::SwapchainKHR m_swapChainHandle;
    std::vector<BadgerEngine::Frame> m_frames;

    bool m_isValid = false;
    std::vector<std::string> m_errors;
public:
    struct Settings {
        vk::SurfaceFormatKHR surfaceFormat;
        vk::Format depthFormat;
        vk::PresentModeKHR presentMode;
        vk::Extent2D extent;
        e172vp::Hardware::SwapChainSupportDetails supportDetails;
    };
private:
    Settings m_settings;
public:

    static vk::SurfaceFormatKHR chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);
    static vk::PresentModeKHR choosePresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);
    static vk::Extent2D chooseExtent(const vk::SurfaceCapabilitiesKHR &capabilities, const vk::Extent2D &defaultExtent = vk::Extent2D());
    static Settings createSettings(vk::PhysicalDevice physicalDevice, const e172vp::Hardware::SwapChainSupportDetails& supportDetails, const vk::Extent2D& defaultExtent = vk::Extent2D());

    static bool createImageViewes(const vk::Device& logicDevice, const std::vector<vk::Image>& swapChainImages, const vk::Format& swapChainImageFormat, std::vector<vk::ImageView>* swapChainImageViews, std::vector<std::string>* error_queue);

    static vk::ImageView createImageView(const vk::Device &logicalDevice, vk::Image image, vk::Format format, std::vector<std::string> *error_queue = nullptr);

    std::vector<vk::ImageView> imageViewVector() const;
    std::vector<vk::Framebuffer> frameBufferVector() const;

    SwapChain() {}

    // SwapChain(const SwapChain&) = delete;
    // SwapChain& operator=(const SwapChain&) = delete;

    SwapChain(const vk::Device& logicalDevice, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const vk::RenderPass& renderPass, const Hardware::QueueFamilies& queueFamilies, const Settings& settings);
    ~SwapChain();
    vk::SwapchainKHR swapChainHandle() const;
    operator vk::SwapchainKHR () const;

    std::vector<std::string> pullErrors();
    bool isValid() const;

    vk::Image image(size_t index) const;

    size_t imageCount() const;
    vk::ImageView imageView(size_t index) const;
    size_t imageViewCount() const;

    const auto& frames() const
    {
        return m_frames;
    }

    Settings settings() const;

private:
    struct ImageInputChunk {
        vk::Device logicalDevice;
        vk::PhysicalDevice physicalDevice;
        int width, height;
        vk::ImageTiling tiling;
        vk::ImageUsageFlags usage;
        vk::MemoryPropertyFlags memoryProperties;
        vk::Format format;
    };

    static vk::Image make_image(ImageInputChunk input);
    static vk::DeviceMemory make_image_memory(ImageInputChunk input, vk::Image image);
    static vk::ImageView make_image_view(
        vk::Device logicalDevice,
        vk::Image image,
        vk::Format format,
        vk::ImageAspectFlags aspect);

    static uint32_t findMemoryTypeIndex(vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties);
    static vk::Format find_supported_depth_format(vk::PhysicalDevice physicalDevice, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

private:
    vk::Device m_logicalDevice;
};

}
