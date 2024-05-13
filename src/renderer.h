#ifndef RENDERER_H
#define RENDERER_H

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#include <GLFW/glfw3.h>

#include "descriptorsetlayout.h"
#include "graphicsobject.h"
#include "pipeline.h"
#include "vertexobject.h"
#include <list>

#include "font.h"

namespace BadgerEngine {
class Model;
}

namespace e172vp {

class Renderer {
public:
    static std::vector<std::string> glfwExtensions();

    static void proceedCommandBuffers(const vk::RenderPass& renderPass,
        const vk::Extent2D& extent,
        const std::vector<vk::Framebuffer>& swapChainFramebuffers,
        const std::vector<vk::CommandBuffer>& commandBuffers,
        const std::vector<vk::DescriptorSet>& uniformDescriptorSets,
        const std::list<VertexObject*>& vertexObjects);

    static void resetCommandBuffers(const std::vector<vk::CommandBuffer> &commandBuffers, const vk::Queue &graphicsQueue, const vk::Queue &presentQueue);
    static void createSyncObjects(const vk::Device& logicDevice, vk::Semaphore* imageAvailableSemaphore, vk::Semaphore* renderFinishedSemaphore);

    VertexObject* addObject(const BadgerEngine::Model& model);

    bool removeVertexObject(VertexObject* vertexObject);
    Renderer();

    bool isAlive() const;
    void applyPresentation();
    void updateUniformBuffer(uint32_t currentImage);

    GraphicsObject graphicsObject() const;

private:
    struct GlobalUniformBufferObject {
        glm::vec2 offset;
        float currentTime;
        glm::vec2 mousePosition;
    };

private:
    VertexObject* addCharacter(char c, std::shared_ptr<Pipeline> pipeline);
    std::shared_ptr<Pipeline> createPipeline(const std::vector<std::uint8_t>& vertShaderCode, const std::vector<std::uint8_t>& fragShaderCode);
    VertexObject* addObject(const BadgerEngine::Geometry::Mesh& mesh, Shared<Pipeline> pipeline);

private:
    static constexpr std::uint32_t WIDTH = 800;
    static constexpr std::uint32_t HEIGHT = 600;

    GraphicsObject m_graphicsObject;
    GLFWwindow* m_window = nullptr;

    vk::Semaphore m_imageAvailableSemaphore;
    vk::Semaphore m_renderFinishedSemaphore;

    vk::Buffer m_vertexBuffer;
    vk::DeviceMemory m_vertexBufferMemory;
    vk::Buffer m_indexBuffer;
    vk::DeviceMemory m_indexBufferMemory;

    e172vp::DescriptorSetLayout m_globalDescriptorSetLayout;
    e172vp::DescriptorSetLayout m_objectDescriptorSetLayout;
    e172vp::DescriptorSetLayout m_samplerDescriptorSetLayout;

    std::vector<vk::Buffer> m_uniformBuffers;
    std::vector<vk::DeviceMemory> m_uniformBuffersMemory;
    std::vector<vk::DescriptorSet> m_uniformDescriptorSets;

    Font* m_font = nullptr;

    std::list<VertexObject*> m_vertexObjects;
};
}

#endif // RENDERER_H
