#pragma once

#include "Buffers/BufferUtils.h"
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
        const std::list<VertexObject*>& vertexObjects);

    static void resetCommandBuffers(const std::vector<vk::CommandBuffer> &commandBuffers, const vk::Queue &graphicsQueue, const vk::Queue &presentQueue);
    static void createSyncObjects(const vk::Device& logicDevice, vk::Semaphore* imageAvailableSemaphore, vk::Semaphore* renderFinishedSemaphore);

    VertexObject* addObject(const BadgerEngine::Model& model);

    bool removeVertexObject(VertexObject* vertexObject);
    Renderer(Shared<Window> window);

    void applyPresentation();
    void updateUniformBuffer(uint32_t currentImage);

    const auto& graphicsObject() const { return m_graphicsObject; }

    Shared<const PerspectiveCamera> camera() const
    {
        return m_camera;
    }

    Shared<PerspectiveCamera> camera()
    {
        return m_camera;
    }

private:
    VertexObject* addCharacter(char c, std::shared_ptr<e172vp::Pipeline> pipeline);
    std::shared_ptr<e172vp::Pipeline> createPipeline(
        const std::vector<std::uint8_t>& vertShaderCode,
        const std::vector<std::uint8_t>& fragShaderCode,
        Geometry::Topology topology);
    VertexObject* addObject(const BadgerEngine::Geometry::Mesh& mesh, Shared<e172vp::Pipeline> pipeline);

private:
    Shared<e172vp::GraphicsObject> m_graphicsObject;

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

    std::list<VertexObject*> m_vertexObjects;

    std::shared_ptr<e172vp::Pipeline> m_normalDebugPipeline;

    Shared<Window> m_window;
    Shared<PerspectiveCamera> m_camera;
};
}
