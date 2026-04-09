#pragma once

#include "../Utils/Error.h"
#include "../Utils/NoNull.h"
#include "SwapChain.h"
#include <vulkan/vulkan.hpp>

namespace BadgerEngine {

class ColorRenderPass {
    struct Private {};

public:
    static Expected<Unique<ColorRenderPass>> create(const vk::Device& logicalDevice, const SwapChain::Settings&) noexcept;

    ColorRenderPass(const ColorRenderPass&) = delete;
    ColorRenderPass(ColorRenderPass&&) = delete;
    ColorRenderPass& operator=(const ColorRenderPass&) = delete;
    ColorRenderPass& operator=(ColorRenderPass&&) = delete;

    ColorRenderPass(vk::RenderPass&& handle, Private)
        : m_handle(std::move(handle))
    {
    }

    const vk::RenderPass& handle() const { return m_handle; }

private:
    vk::RenderPass m_handle;
};

}
