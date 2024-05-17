#pragma once

#include "Tools/buffer.h"
#include "Utils/NoNull.h"
#include "descriptorsetlayout.h"
#include <glm/glm.hpp>
#include <memory>

namespace BadgerEngine {

class Renderer;
namespace Geometry {
class Mesh;
}

}

namespace e172vp {

class GraphicsObject;
class Pipeline;

class VertexObject {
    friend BadgerEngine::Renderer;

    static constexpr glm::mat4 sm = {
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
    };

    ~VertexObject();
    VertexObject(
        const e172vp::GraphicsObject* graphicsObject,
        size_t imageCount,
        const DescriptorSetLayout* descriptorSetLayout,
        const DescriptorSetLayout* samplerDescriptorSetLayout,
        const BadgerEngine::Geometry::Mesh& mesh,
        const vk::ImageView& imageView,
        BadgerEngine::Shared<Pipeline> pipeline);

public:
    GraphicsObject *graphicsObject() const;

    std::vector<BadgerEngine::BufferBundle> bufferBundles() const
    {
        return m_uniformBufferBundles;
    }

    vk::Buffer vertexBuffer() const;
    vk::Buffer indexBuffer() const;
    uint32_t indexCount() const;
    const auto& pipeline() const { return m_pipeline; }

    void updateUbo(int imageIndex);
    glm::mat4 rotation() const;
    void setRotation(const glm::mat4 &rotation);
    glm::mat4 translation() const;
    void setTranslation(const glm::mat4 &translation);
    glm::mat4 scale() const;
    void setScale(const glm::mat4 &scale);
    std::vector<vk::DescriptorSet> textureDescriptorSets() const;

private:
    glm::mat4 m_rotation = sm;
    glm::mat4 m_translation = sm;
    glm::mat4 m_scale = sm;

    GraphicsObject* m_graphicsObject = nullptr;

    vk::Buffer m_vertexBuffer;
    vk::DeviceMemory m_vertexBufferMemory;
    vk::Buffer m_indexBuffer;
    vk::DeviceMemory m_indexBufferMemory;

    std::vector<BadgerEngine::BufferBundle> m_uniformBufferBundles;

    std::vector<vk::DescriptorSet> m_textureDescriptorSets;
    std::uint32_t m_indexCount;
    BadgerEngine::Shared<Pipeline> m_pipeline;
};

}
