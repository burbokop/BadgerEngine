#pragma once

#include "Buffers/BufferUtils.h"
#include "RenderingOptions.h"
#include "descriptorsetlayout.h"
#include "font.h"
#include "graphicsobject.h"
#include "pipeline.h"
#include <filesystem>
#include <list>

namespace BadgerEngine {

class Model;
class Camera;
class Window;
struct PointLight;
class UploadedTexture;
class VertexObject;
class UploadedTextureCache;

class Renderer {

public:
    VertexObject& addObject(const BadgerEngine::Model& model, RenderingOptions options = RenderingOptions());

    Shared<PointLight> addPointLight(
        glm::vec3 position,
        glm::vec3 color,
        float intensity);

    bool removeVertexObject(VertexObject* vertexObject);

    Renderer(Shared<Window> window, Shared<Camera> camera, std::span<const uint8_t> fontBytes);

    [[nodiscard]] Expected<void> applyPresentation() noexcept;

    Shared<const Camera> camera() const
    {
        return m_camera;
    }

    Shared<Camera> camera()
    {
        return m_camera;
    }

    void setDirectionalLightVector(glm::vec3 v)
    {
        m_directionalLightVector = v;
    }

    glm::vec3 directionalLightVector() const { return m_directionalLightVector; }

    void setDirectionalLightColor(glm::vec3 c)
    {
        m_directionalLightColor = c;
    }

    glm::vec3 directionalLightColor() const { return m_directionalLightColor; }

    void setDirectionalLightIntensity(float i)
    {
        m_directionalLightIntensity = i;
    };

    float directionalLightIntensity() const { return m_directionalLightIntensity; }

private:
    void updateUniformBuffer(uint32_t currentImage);

    const auto& graphicsObject() const { return m_graphicsObject; }

    Expected<void> proceedCommandBuffers(const vk::RenderPass& renderPass,
        const vk::Extent2D& extent,
        const Camera& camera,
        const std::vector<vk::Framebuffer>& swapChainFramebuffers,
        const std::vector<vk::CommandBuffer>& commandBuffers,
        const std::vector<BufferBundle>& commonGlobalUniformBufferBundles,
        const std::vector<BufferBundle>& lightingUniformBufferBundles,
        const std::list<VertexObject*>& vertexObjects);

    VertexObject& addCharacter(char c, std::shared_ptr<e172vp::Pipeline> pipeline);
    std::shared_ptr<e172vp::Pipeline> createPipeline(
        std::span<const std::uint8_t> vertShaderCode,
        std::span<const std::uint8_t> fragShaderCode,
        Geometry::Topology topology,
        BadgerEngine::PolygonMode polygonMode,
        bool backfaceCulling);

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
    e172vp::DescriptorSetLayout m_baseColorSamplerDescriptorSetLayout;
    e172vp::DescriptorSetLayout m_ambientOcclusionDescriptorSetLayout;
    e172vp::DescriptorSetLayout m_normalMapDescriptorSetLayout;

    std::vector<BufferBundle> m_commonGlobalUniformBufferBundles;
    std::vector<BufferBundle> m_lightingUniformBufferBundles;

    e172vp::Font* m_font = nullptr;

    std::list<VertexObject*> m_vertexObjects;

    std::shared_ptr<e172vp::Pipeline> m_normalDebugPipeline;

    std::vector<Shared<PointLight>> m_pointLights;

    glm::vec3 m_directionalLightVector = glm::vec3(0.);
    glm::vec3 m_directionalLightColor = glm::vec3(1.f);
    float m_directionalLightIntensity = 0.f;

    Shared<Window> m_window;
    Shared<Camera> m_camera;
    Shared<UploadedTextureCache> m_textureCache;
};

}
