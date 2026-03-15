#pragma once

#include "Buffers/BufferUtils.h"
#include "Buffers/MeshBuffer.h"
#include "Tools/UploadedModel.h"
#include "VertexObject.h"
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

class BSDFVertexObject : public VertexObject {
public:
    ~BSDFVertexObject();
    BSDFVertexObject(
        Shared<e172vp::GraphicsObject> graphicsObject,
        std::size_t imageCount,
        const e172vp::DescriptorSetLayout& uniformBufferDescriptorSetLayout,
        const e172vp::DescriptorSetLayout& baseColorDescriptorSetLayout,
        const e172vp::DescriptorSetLayout& ambientOclussionDescriptorSetLayout,
        const Shared<BadgerEngine::Geometry::Mesh>& mesh,
        Shared<UploadedTexture> baseColorTexture,
        Shared<UploadedTexture> ambientOclussionMap,
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

protected:
    Expected<void> draw(std::size_t imageIndex,
        std::span<const vk::CommandBuffer> commandBuffers,
        std::span<const BufferBundle> commonGlobalUniformBufferBundles,
        std::span<const BufferBundle> lightingUniformBufferBundles) const noexcept override;

    Expected<void> updateUniformBuffer(std::size_t imageIndex) noexcept override;

private:
    Shared<e172vp::GraphicsObject> m_graphicsObject;
    std::vector<UploadedModel> m_models;
    std::optional<Shared<UploadedTexture>> m_baseColorTexture;
    std::optional<Shared<UploadedTexture>> m_ambientOclussionMap;

    std::vector<BadgerEngine::BufferBundle> m_uniformBufferBundles;
    std::vector<vk::DescriptorSet> m_baseColorTextureDescriptorSets;
    std::vector<vk::DescriptorSet> m_ambientOclussionMapDescriptorSets;
};

}
