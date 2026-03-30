#pragma once

#include "Geometry/Topology.h"
#include "Model/Model.h"
#include "RenderingOptions.h"
#include "Utils/Error.h"
#include "Utils/NoNull.h"
#include "VertexObject.h"
#include <glm/glm.hpp>
#include <list>
#include <vector>

namespace vk {

class Framebuffer;
class Extent2D;
class RenderPass;
class CommandBuffer;
union ClearValue;

}

namespace e172vp {

class GraphicsObject;
class Pipeline;
class Font;

}

namespace BadgerEngine {

struct PointLight;
struct BufferBundle;
class Model;
class Camera;
class Window;
class UploadedTexture;
class UploadedTextureCache;

class Renderer {
public:
    ~Renderer();
    Renderer(Shared<Window> window, Shared<Camera> camera, std::span<const uint8_t> fontBytes);

    VertexObject& addObject(const BadgerEngine::Model& model, RenderingOptions options = RenderingOptions());

    Shared<PointLight> addPointLight(
        glm::vec3 position,
        glm::vec3 color,
        float intensity);

    bool removeVertexObject(VertexObject* vertexObject);

    [[nodiscard]] Expected<void> applyPresentation() noexcept;

    Shared<const Camera> camera() const
    {
        return m_camera;
    }

    Shared<Camera> camera()
    {
        return m_camera;
    }

    void setDirectionalLightVector(glm::vec3 v);

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

    [[nodiscard]] Expected<void> proceedRenderPass(
        std::size_t imageIndex,
        const vk::RenderPass& renderPass,
        const vk::Extent2D& extent,
        const Camera& camera,
        std::span<const vk::ClearValue> clearValues,
        const std::vector<vk::Framebuffer>& swapChainFramebuffers,
        const std::vector<vk::CommandBuffer>& commandBuffers,
        const std::vector<BufferBundle>& commonGlobalUniformBufferBundles,
        const std::vector<BufferBundle>& lightingUniformBufferBundles,
        const std::list<VertexObject*>& vertexObjects,
        VertexObject::RenderTarget renderTarget) noexcept;

    [[nodiscard]] Expected<void> fillCommandBuffers();

    VertexObject& addCharacter(char c, std::shared_ptr<e172vp::Pipeline> pipeline);
    std::shared_ptr<e172vp::Pipeline> createColorPipeline(
        std::span<const std::uint8_t> vertShaderCode,
        std::span<const std::uint8_t> fragShaderCode,
        Geometry::Topology topology,
        BadgerEngine::PolygonMode polygonMode,
        bool backfaceCulling);

    std::shared_ptr<e172vp::Pipeline> createShadowMapAsColorPipeline(
        std::span<const std::uint8_t> vertShaderCode,
        std::span<const std::uint8_t> fragShaderCode,
        Geometry::Topology topology,
        BadgerEngine::PolygonMode polygonMode,
        bool backfaceCulling);

    std::shared_ptr<e172vp::Pipeline> createShadowMapPipeline(
        std::span<const std::uint8_t> vertShaderCode,
        Geometry::Topology topology,
        BadgerEngine::PolygonMode polygonMode,
        bool backfaceCulling);

private:
    struct Impl;

private:
    Unique<Impl> m_impl;
    Shared<Camera> m_camera;
    glm::vec3 m_directionalLightVector = glm::vec3(0.);
    glm::vec3 m_directionalLightColor = glm::vec3(1.f);
    float m_directionalLightIntensity = 0.f;
};

}
