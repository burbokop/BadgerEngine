#include "ShadowMapRenderPass.h"

namespace BadgerEngine {

Expected<Unique<ShadowMapRenderPass>> ShadowMapRenderPass::create(const vk::Device& logicalDevice, const e172vp::SwapChain::Settings& swapchainSettings) noexcept
{
    (void)logicalDevice;
    (void)swapchainSettings;
    return unexpected("TODO");
}

}
