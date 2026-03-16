#pragma once

#include "Buffers/BufferUtils.h"
#include "Buffers/MeshBuffer.h"
#include "Tools/UploadedModel.h"
#include "descriptorsetlayout.h"
#include <glm/glm.hpp>
#include <span>

namespace e172vp {
class GraphicsObject;
class Pipeline;
};

namespace BadgerEngine {

class Renderer;
class UploadedTexture;

namespace Geometry {
class Mesh;
}

class VertexObject {
    friend BadgerEngine::Renderer;

public:
    VertexObject() = default;
    virtual ~VertexObject() = default;

    VertexObject& operator=(VertexObject&&) = delete;
    VertexObject& operator=(const VertexObject&) = delete;
    VertexObject(VertexObject&&) = delete;
    VertexObject(const VertexObject&) = delete;

    glm::mat4 rotation() const;
    VertexObject& setRotation(const glm::mat4& rotation);
    glm::mat4 translation() const;
    VertexObject& setTranslation(const glm::mat4& translation);
    VertexObject& setTranslation(const glm::vec3& translation);
    glm::mat4 scale() const;
    VertexObject& setScale(const glm::mat4& scale);
    VertexObject& setScale(const glm::vec3& scale);
    std::vector<vk::DescriptorSet> textureDescriptorSets() const;

protected:
    [[nodiscard]] virtual Expected<void> draw(std::size_t imageIndex,
        std::span<const vk::CommandBuffer> commandBuffers,
        std::span<const BufferBundle> commonGlobalUniformBufferBundles,
        std::span<const BufferBundle> lightingUniformBufferBundles) const noexcept
        = 0;

    [[nodiscard]] virtual Expected<void> updateUniformBuffer(std::size_t imageIndex) noexcept = 0;

private:
    glm::mat4 m_rotation = glm::mat4(1.);
    glm::mat4 m_translation = glm::mat4(1.);
    glm::mat4 m_scale = glm::mat4(1.);
};

}
