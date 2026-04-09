#pragma once

#include "../Utils/Error.h"
#include "hardware.h"
#include <vulkan/vulkan.hpp>

namespace BadgerEngine {

struct ColorRenderPassFrame {
    vk::Image image;
    vk::ImageView imageView;
    vk::Image depthBuffer;
    vk::DeviceMemory depthBufferMemory;
    vk::ImageView depthBufferView;
    vk::Framebuffer framebuffer;
};

struct ShadowMapRenderPassFrame {
    vk::Image image;
    vk::DeviceMemory memory;
    vk::ImageView imageView;
    vk::Framebuffer framebuffer;
};

struct Frame {
    ColorRenderPassFrame color;
    ShadowMapRenderPassFrame shadowMap;
};

Expected<vk::ImageView> createImageView(const vk::Device& logicalDevice, vk::Image image, vk::Format format) noexcept;

class SwapChain {
public:
    struct Settings {
        vk::SurfaceFormatKHR surfaceFormat;
        vk::Format depthFormat;
        vk::PresentModeKHR presentMode;
        vk::Extent2D extent;
        vk::Extent2D shadowMapExtent;
        e172vp::Hardware::SwapChainSupportDetails supportDetails;
    };

public:
    SwapChain(const SwapChain&) = delete;
    SwapChain(SwapChain&&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;
    SwapChain& operator=(SwapChain&&) = delete;

    SwapChain(
        const vk::Device& logicalDevice,
        const vk::PhysicalDevice& physicalDevice,
        const vk::SurfaceKHR& surface,
        const vk::RenderPass& colorRenderPass,
        const vk::RenderPass& shadowMapRenderPass,
        const e172vp::Hardware::QueueFamilies& queueFamilies,
        const Settings& settings);

    ~SwapChain();

    static Settings createSettings(vk::PhysicalDevice physicalDevice, const e172vp::Hardware::SwapChainSupportDetails& supportDetails, const vk::Extent2D& defaultExtent = vk::Extent2D());
    std::vector<vk::ImageView> colorImageViewVector() const;
    std::vector<vk::ImageView> shadowMapImageViewVector() const;
    std::vector<vk::Framebuffer> frameBufferVector() const;
    std::vector<vk::Framebuffer> shadowMapFrameBufferVector() const;
    std::vector<std::string> pullErrors();
    bool isValid() const;
    vk::Image image(size_t index) const;
    size_t imageCount() const;
    vk::ImageView imageView(size_t index) const;
    size_t imageViewCount() const;
    Settings settings() const;

    const auto& frames() const
    {
        return m_frames;
    }

    const vk::SwapchainKHR& swapChainHandle() const
    {
        return m_swapChainHandle;
    }

private:
    vk::Device m_logicalDevice;
    vk::SwapchainKHR m_swapChainHandle;
    std::vector<BadgerEngine::Frame> m_frames;
    bool m_isValid = false;
    std::vector<std::string> m_errors;
    Settings m_settings;
};

}
