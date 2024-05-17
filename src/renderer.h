#pragma once

#include "Tools/buffer.h"
#include "descriptorsetlayout.h"
#include "graphicsobject.h"
#include "pipeline.h"
#include "vertexobject.h"
#include <list>

#include "font.h"

namespace BadgerEngine {

class Model;
class PerspectiveCamera;
class Window;

class Renderer {

public:
    static void proceedCommandBuffers(
        const vk::RenderPass& renderPass,
        const vk::Extent2D& extent,
        const PerspectiveCamera& camera,
        const std::vector<vk::Framebuffer>& swapChainFramebuffers,
        const std::vector<vk::CommandBuffer>& commandBuffers,
        const std::vector<BufferBundle>& commonGlobalUniformBufferBundles,
        const std::vector<BufferBundle>& lightingUniformBufferBundles,
        const std::list<e172vp::VertexObject*>& vertexObjects);

    static void resetCommandBuffers(const std::vector<vk::CommandBuffer> &commandBuffers, const vk::Queue &graphicsQueue, const vk::Queue &presentQueue);
    static void createSyncObjects(const vk::Device& logicDevice, vk::Semaphore* imageAvailableSemaphore, vk::Semaphore* renderFinishedSemaphore);

    e172vp::VertexObject* addObject(const BadgerEngine::Model& model);

    bool removeVertexObject(e172vp::VertexObject* vertexObject);
    Renderer(Shared<Window> window);

    void applyPresentation();
    void updateUniformBuffer(uint32_t currentImage);

    e172vp::GraphicsObject graphicsObject() const;

    Shared<const PerspectiveCamera> camera() const
    {
        return m_camera;
    }

    Shared<PerspectiveCamera> camera()
    {
        return m_camera;
    }

private:
    e172vp::VertexObject* addCharacter(char c, std::shared_ptr<e172vp::Pipeline> pipeline);
    std::shared_ptr<e172vp::Pipeline> createPipeline(const std::vector<std::uint8_t>& vertShaderCode, const std::vector<std::uint8_t>& fragShaderCode);
    e172vp::VertexObject* addObject(const BadgerEngine::Geometry::Mesh& mesh, Shared<e172vp::Pipeline> pipeline);

private:
    e172vp::GraphicsObject m_graphicsObject;

    vk::Semaphore m_imageAvailableSemaphore;
    vk::Semaphore m_renderFinishedSemaphore;

    vk::Buffer m_vertexBuffer;
    vk::DeviceMemory m_vertexBufferMemory;
    vk::Buffer m_indexBuffer;
    vk::DeviceMemory m_indexBufferMemory;

    e172vp::DescriptorSetLayout m_globalDescriptorSetLayout;
    e172vp::DescriptorSetLayout m_lightingDescriptorSetLayout;
    e172vp::DescriptorSetLayout m_objectDescriptorSetLayout;
    e172vp::DescriptorSetLayout m_samplerDescriptorSetLayout;

    std::vector<BufferBundle> m_commonGlobalUniformBufferBundles;
    std::vector<BufferBundle> m_lightingUniformBufferBundles;

    e172vp::Font* m_font = nullptr;

    std::list<e172vp::VertexObject*> m_vertexObjects;

    Shared<Window> m_window;
    Shared<PerspectiveCamera> m_camera;
};
}
