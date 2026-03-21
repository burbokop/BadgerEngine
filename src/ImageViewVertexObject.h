#pragma once

#include "Buffers/BufferUtils.h"
#include "Buffers/MeshBuffer.h"
#include "RenderingOptions.h"
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

class ImageViewVertexObject : public VertexObject {

public:
    ~ImageViewVertexObject();

    ImageViewVertexObject(
        Shared<e172vp::GraphicsObject> graphicsObject,
        std::size_t imageCount,
        const e172vp::DescriptorSetLayout& uniformBufferDescriptorSetLayout,
        const e172vp::DescriptorSetLayout& samplerDescriptorSetLayout,
        const Shared<BadgerEngine::Geometry::Mesh>& mesh,
        const vk::ImageView& imageView,
        Shared<e172vp::Pipeline> pipeline,
        Shared<e172vp::Pipeline> nPipeline,
        DisplayNormals displayNormals);

    ImageViewVertexObject(
        Shared<e172vp::GraphicsObject> graphicsObject,
        std::size_t imageCount,
        const e172vp::DescriptorSetLayout& uniformBufferDescriptorSetLayout,
        const e172vp::DescriptorSetLayout& samplerDescriptorSetLayout,
        const Shared<BadgerEngine::Geometry::Mesh>& mesh,
        Shared<UploadedTexture> texture,
        Shared<e172vp::Pipeline> pipeline,
        Shared<e172vp::Pipeline> nPipeline,
        DisplayNormals displayNormals);

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
    std::optional<Shared<UploadedTexture>> m_texture;

    std::vector<BadgerEngine::BufferBundle> m_uniformBufferBundles;
    std::vector<vk::DescriptorSet> m_textureDescriptorSets;
    std::vector<vk::DescriptorSet> m_ambientOclussionMapDescriptorSets;
};

}
