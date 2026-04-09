#include "ShadowMapRenderPass.h"

namespace BadgerEngine {

Expected<Unique<ShadowMapRenderPass>> ShadowMapRenderPass::create(const vk::Device& logicalDevice, const SwapChain::Settings& swapchainSettings) noexcept
{
    const vk::AttachmentDescription depthStencilAttachment = {
        .format = swapchainSettings.depthFormat,
        .samples = vk::SampleCountFlagBits::e1,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal,
    };

    const vk::AttachmentReference depthStencilAttachmentRef = {
        .attachment = 0,
        .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    const vk::SubpassDescription subpass = {
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
        .colorAttachmentCount = 0,
        .pColorAttachments = {},
        .pDepthStencilAttachment = &depthStencilAttachmentRef,
    };

    const std::array attachments = {
        depthStencilAttachment
    };

    const std::array<vk::SubpassDependency, 2> dependencies {
        vk::SubpassDependency {
            .srcSubpass = vk::SubpassExternal,
            .dstSubpass = 0,
            .srcStageMask = vk::PipelineStageFlagBits::eFragmentShader,
            .dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests,
            .srcAccessMask = vk::AccessFlagBits::eShaderRead,
            .dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite,
            .dependencyFlags = vk::DependencyFlagBits::eByRegion,
        },
        vk::SubpassDependency {
            .srcSubpass = 0,
            .dstSubpass = vk::SubpassExternal,
            .srcStageMask = vk::PipelineStageFlagBits::eLateFragmentTests,
            .dstStageMask = vk::PipelineStageFlagBits::eFragmentShader,
            .srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite,
            .dstAccessMask = vk::AccessFlagBits::eShaderRead,
            .dependencyFlags = vk::DependencyFlagBits::eByRegion,
        }
    };

    const vk::RenderPassCreateInfo renderPassCreateInfo = {
        .attachmentCount = attachments.size(),
        .pAttachments = attachments.data(),
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = dependencies.size(),
        .pDependencies = dependencies.data(),
    };

    vk::RenderPass handle;
    const auto code = logicalDevice.createRenderPass(&renderPassCreateInfo, nullptr, &handle);
    if (code != vk::Result::eSuccess) {
        return unexpected("Failed to create render pass: " + vk::to_string(code));
    }

    assert(handle);

    return std::make_unique<ShadowMapRenderPass>(std::move(handle), Private {});
}

}
