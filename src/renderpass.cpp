#include "renderpass.h"
#include <iostream>

vk::RenderPass e172vp::RenderPass::renderPathHandle() const { return m_renderPathHandle; }

e172vp::RenderPass::operator VkRenderPass() const {
    return m_renderPathHandle;
}

bool e172vp::RenderPass::isValid() const { return m_isValid; }

e172vp::RenderPass::RenderPass(const vk::Device& logicalDevice, const SwapChain::Settings& swapchainSettings)
{
    const auto colorAttachment = [&] {
        vk::AttachmentDescription attachment;
        attachment.format = swapchainSettings.surfaceFormat.format;
        attachment.samples = vk::SampleCountFlagBits::e1;
        attachment.loadOp = vk::AttachmentLoadOp::eClear;
        attachment.storeOp = vk::AttachmentStoreOp::eStore;
        attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        attachment.initialLayout = vk::ImageLayout::eUndefined;
        attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
        return attachment;
    }();

    const auto colorAttachmentRef = [&] {
        vk::AttachmentReference ref;
        ref.attachment = 0;
        ref.layout = vk::ImageLayout::eColorAttachmentOptimal;
        return ref;
    }();

    const auto depthStencilAttachment = [&] {
        vk::AttachmentDescription attachment;
        attachment.format = swapchainSettings.depthFormat;
        attachment.samples = vk::SampleCountFlagBits::e1;
        attachment.loadOp = vk::AttachmentLoadOp::eClear;
        attachment.storeOp = vk::AttachmentStoreOp::eStore;
        attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        attachment.initialLayout = vk::ImageLayout::eUndefined;
        attachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        return attachment;
    }();

    const auto depthStencilAttachmentRef = [&] {
        vk::AttachmentReference ref;
        ref.attachment = 1;
        ref.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        return ref;
    }();

    const auto subpass = [&] {
        vk::SubpassDescription subpass;
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthStencilAttachmentRef;
        return subpass;
    }();

    const std::array attachments = {
        colorAttachment,
        depthStencilAttachment
    };

    const auto renderPassCreateInfo = [&] {
        vk::RenderPassCreateInfo info;
        info.attachmentCount = attachments.size();
        info.pAttachments = attachments.data();
        info.subpassCount = 1;
        info.pSubpasses = &subpass;
        return info;
    }();

    const auto code = logicalDevice.createRenderPass(&renderPassCreateInfo, nullptr, &m_renderPathHandle);
    if (code != vk::Result::eSuccess) {
        m_errors.push_back("[error] Failed to create render pass: " + vk::to_string(code));
        return;
    }
    assert(m_renderPathHandle);

    m_isValid = true;
}

std::vector<std::string> e172vp::RenderPass::pullErrors() {
    const auto r = m_errors;
    m_errors.clear();
    return r;
}

e172vp::RenderPass::operator vk::RenderPass() const {
    return m_renderPathHandle;
}
