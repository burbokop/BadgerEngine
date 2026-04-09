#pragma once

#include "../Utils/Error.h"
#include "../Utils/NoNull.h"
#include "SwapChain.h"
#include <vulkan/vulkan.hpp>

namespace BadgerEngine {

class ShadowMapRenderPass {
    struct Private {};

public:
    static Expected<Unique<ShadowMapRenderPass>> create(const vk::Device& logicalDevice, const SwapChain::Settings&) noexcept;

    ShadowMapRenderPass(const ShadowMapRenderPass&) = delete;
    ShadowMapRenderPass(ShadowMapRenderPass&&) = delete;
    ShadowMapRenderPass& operator=(const ShadowMapRenderPass&) = delete;
    ShadowMapRenderPass& operator=(ShadowMapRenderPass&&) = delete;

    ShadowMapRenderPass(vk::RenderPass&& handle, Private)
        : m_handle(std::move(handle))
    {
    }

    vk::RenderPass handle() const { return m_handle; }

private:
    vk::RenderPass m_handle;
};

}
