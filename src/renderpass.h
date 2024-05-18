#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <vulkan/vulkan.hpp>
#include "swapchain.h"

namespace e172vp {


class RenderPass {
    vk::RenderPass m_renderPathHandle;
    std::vector<std::string> m_errors;
    bool m_isValid = false;
public:
    RenderPass() {}
    RenderPass(const vk::Device& logicalDevice, const SwapChain::Settings&);
    vk::RenderPass renderPathHandle() const;
    operator vk::RenderPass() const;
    operator VkRenderPass() const;
    std::vector<std::string> pullErrors();
    bool isValid() const;
};

}
#endif // RENDERPASS_H
