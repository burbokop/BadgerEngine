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

struct RendererImpl;

class DirectionalLight {
public:
    DirectionalLight(Shared<RendererImpl> impl)
        : m_impl(std::move(impl))
    {
    }

    void setDirection(glm::vec3 v);

    glm::vec3 direction() const
    {
        return m_direction;
    }

    void setColor(glm::vec3 c)
    {
        m_color = c;
    }

    glm::vec3 color() const { return m_color; }

    void setIntensity(float i)
    {
        m_intensity = i;
    };

    float intensity() const { return m_intensity; }

    void setShadowFocus(glm::vec3 v);

    glm::vec3 shadowFocus() const
    {
        return m_shadowFocus;
    }

    glm::vec3 shadowCameraPosition() const;

    void setShadowNear(float v);
    void setShadowFar(float v);
    void setShadowCameraScale(float v);

private:
    Shared<RendererImpl> m_impl;
    glm::vec3 m_direction = glm::vec3(0.);
    glm::vec3 m_color = glm::vec3(1.f);
    float m_intensity = 0.f;
    glm::vec3 m_shadowFocus;
};

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

    void setMode(std::uint32_t mode)
    {
        m_mode = mode;
    }

    DirectionalLight& directionalLight();
    const DirectionalLight& directionalLight() const;

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
    Shared<RendererImpl> m_impl;
    Shared<Camera> m_camera;
    std::uint32_t m_mode = 0;
    DirectionalLight m_directionalLight;
};

}
