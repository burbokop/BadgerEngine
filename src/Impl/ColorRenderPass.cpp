#include "ColorRenderPass.h"

namespace BadgerEngine {

Expected<Unique<ColorRenderPass>> ColorRenderPass::create(const vk::Device& logicalDevice, const SwapChain::Settings& swapchainSettings) noexcept
{
    const vk::AttachmentDescription colorAttachment = {
        .format = swapchainSettings.surfaceFormat.format,
        .samples = vk::SampleCountFlagBits::e1,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::ePresentSrcKHR,
    };

    const vk::AttachmentReference colorAttachmentRef = {
        .attachment = 0,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    };

    const vk::AttachmentDescription depthStencilAttachment = {
        .format = swapchainSettings.depthFormat,
        .samples = vk::SampleCountFlagBits::e1,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    const vk::AttachmentReference depthStencilAttachmentRef = {
        .attachment = 1,
        .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    const vk::SubpassDescription subpass = {
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef,
        .pDepthStencilAttachment = &depthStencilAttachmentRef,
    };

    const std::array attachments = {
        colorAttachment,
        depthStencilAttachment
    };

    const vk::RenderPassCreateInfo renderPassCreateInfo = {
        .attachmentCount = attachments.size(),
        .pAttachments = attachments.data(),
        .subpassCount = 1,
        .pSubpasses = &subpass,
    };

    vk::RenderPass handle;
    const auto code = logicalDevice.createRenderPass(&renderPassCreateInfo, nullptr, &handle);
    if (code != vk::Result::eSuccess) {
        return unexpected("Failed to create render pass: " + vk::to_string(code));
    }

    assert(handle);

    return std::make_unique<ColorRenderPass>(std::move(handle), Private {});
}

}
