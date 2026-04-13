#pragma once

#include "../RenderingOptions.h"
#include "../VertexObject.h"
#include "Buffers/BufferUtils.h"
#include "Uploaded/UploadedModel.h"
#include "Uploaded/UploadedTexture.h"
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

class ImageViewVertexObject : public VertexObject {

public:
    ImageViewVertexObject(const ImageViewVertexObject &) = delete;
    ImageViewVertexObject(ImageViewVertexObject &&) = delete;
    ImageViewVertexObject &operator=(const ImageViewVertexObject &) = delete;
    ImageViewVertexObject &operator=(ImageViewVertexObject &&) = delete;

    ~ImageViewVertexObject();

    ImageViewVertexObject(
        Shared<e172vp::GraphicsObject> graphicsObject,
        std::size_t imageCount,
        const e172vp::DescriptorSetLayout& uniformBufferDescriptorSetLayout,
        const e172vp::DescriptorSetLayout& baseColorDescriptorSetLayout,
        const Shared<BadgerEngine::Geometry::Mesh>& mesh,
        UploadedMeshCache& cache,
        const vk::ImageView& imageView,
        Shared<e172vp::Pipeline> pipeline,
        Shared<e172vp::Pipeline> nPipeline,
        DisplayNormals displayNormals);

    ImageViewVertexObject(
        Shared<e172vp::GraphicsObject> graphicsObject,
        const e172vp::DescriptorSetLayout& uniformBufferDescriptorSetLayout,
        const e172vp::DescriptorSetLayout& baseColorDescriptorSetLayout,
        const Shared<BadgerEngine::Geometry::Mesh>& mesh,
        UploadedMeshCache& cache,
        std::span<const vk::ImageView> imageViews,
        vk::ImageLayout imageLayout,
        Shared<e172vp::Pipeline> pipeline,
        Shared<e172vp::Pipeline> nPipeline,
        DisplayNormals displayNormals);

    ImageViewVertexObject(
        Shared<e172vp::GraphicsObject> graphicsObject,
        std::size_t imageCount,
        const e172vp::DescriptorSetLayout& uniformBufferDescriptorSetLayout,
        const e172vp::DescriptorSetLayout& baseColorDescriptorSetLayout,
        const Shared<BadgerEngine::Geometry::Mesh>& mesh,
        UploadedMeshCache& cache,
        UploadedTexture texture,
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
    [[nodiscard]] Expected<void> draw(
        std::size_t imageIndex,
        std::span<const vk::CommandBuffer> commandBuffers,
        std::span<const BufferBundle> commonGlobalUniformBufferBundles,
        std::span<const BufferBundle> lightingUniformBufferBundles,
        RenderTarget target) const noexcept override;

    [[nodiscard]] Expected<void> updateUniformBuffer(std::size_t imageIndex) noexcept override;

private:
    Shared<e172vp::GraphicsObject> m_graphicsObject;
    std::vector<UploadedModel> m_models;
    std::optional<UploadedTexture> m_texture;

    std::vector<BadgerEngine::BufferBundle> m_uniformBufferBundles;
    std::vector<vk::DescriptorSet> m_baseColorTextureDescriptorSets;
};

}
