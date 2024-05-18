#pragma once

#include "Buffers/BufferUtils.h"
#include "Buffers/MeshBuffer.h"
#include "Tools/UploadedModel.h"
#include "Utils/NoNull.h"
#include "descriptorsetlayout.h"
#include <glm/glm.hpp>
#include <span>

namespace e172vp {
class GraphicsObject;
class Pipeline;
};

namespace BadgerEngine {

class Renderer;
namespace Geometry {
class Mesh;
}

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
        Shared<e172vp::GraphicsObject> graphicsObject,
        size_t imageCount,
        const e172vp::DescriptorSetLayout* descriptorSetLayout,
        const e172vp::DescriptorSetLayout* samplerDescriptorSetLayout,
        const BadgerEngine::Geometry::Mesh& mesh,
        const vk::ImageView& imageView,
        Shared<e172vp::Pipeline> pipeline,
        Shared<e172vp::Pipeline> nPipeline);

public:
    const auto& graphicsObject() const
    {
        return m_graphicsObject;
    }

    std::vector<BadgerEngine::BufferBundle> bufferBundles() const
    {
        return m_uniformBufferBundles;
    }

    void updateUbo(int imageIndex);
    glm::mat4 rotation() const;
    void setRotation(const glm::mat4 &rotation);
    glm::mat4 translation() const;
    void setTranslation(const glm::mat4 &translation);
    glm::mat4 scale() const;
    void setScale(const glm::mat4 &scale);
    std::vector<vk::DescriptorSet> textureDescriptorSets() const;

    void draw(std::size_t imageIndex,
        std::span<const vk::CommandBuffer> commandBuffers,
        std::span<const BufferBundle> commonGlobalUniformBufferBundles,
        std::span<const BufferBundle> lightingUniformBufferBundles) const;

private:
    glm::mat4 m_rotation = sm;
    glm::mat4 m_translation = sm;
    glm::mat4 m_scale = sm;

    Shared<e172vp::GraphicsObject> m_graphicsObject;
    std::vector<UploadedModel> m_models;

    std::vector<BadgerEngine::BufferBundle> m_uniformBufferBundles;
    std::vector<vk::DescriptorSet> m_textureDescriptorSets;
};

}
