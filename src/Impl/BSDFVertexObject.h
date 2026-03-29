#pragma once

#include "../VertexObject.h"
#include "Buffers/BufferUtils.h"
#include "Buffers/MeshBuffer.h"
#include "UploadedModel.h"
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

enum class DisplayNormals : std::uint8_t;

class BSDFVertexObject : public VertexObject {
public:
    ~BSDFVertexObject();
    BSDFVertexObject(
        Shared<e172vp::GraphicsObject> graphicsObject,
        std::size_t imageCount,
        const e172vp::DescriptorSetLayout& uniformBufferDescriptorSetLayout,
        const e172vp::DescriptorSetLayout& baseColorDescriptorSetLayout,
        const e172vp::DescriptorSetLayout& ambientOclussionDescriptorSetLayout,
        const e172vp::DescriptorSetLayout& normalMapDescriptorSetLayout,
        const e172vp::DescriptorSetLayout& shadowMapSamplerDescriptorSetLayout,
        const Shared<BadgerEngine::Geometry::Mesh>& mesh,
        Shared<UploadedTexture> baseColorTexture,
        Shared<UploadedTexture> ambientOclussionMap,
        Shared<UploadedTexture> normalMap,
        Shared<e172vp::Pipeline> pipeline,
        Shared<e172vp::Pipeline> normalesPipeline,
        Shared<e172vp::Pipeline> shadowMapPipeline,
        DisplayNormals displayNormals);

public:
    const auto& graphicsObject() const { return m_graphicsObject; }

    std::vector<BadgerEngine::BufferBundle> bufferBundles() const
    {
        return m_uniformBufferBundles;
    }

protected:
    [[nodiscard]] Expected<void> draw(
        std::size_t imageIndex,
        std::span<const vk::CommandBuffer> commandBuffers,
        std::span<const BufferBundle> globalUniformBufferBundles,
        std::span<const BufferBundle> lightingUniformBufferBundles,
        RenderTarget target) const noexcept override;

    [[nodiscard]] Expected<void> updateUniformBuffer(std::size_t imageIndex) noexcept override;

private:
    [[nodiscard]] Expected<void> drawColorTarget(
        std::size_t imageIndex,
        std::span<const vk::CommandBuffer> commandBuffers,
        std::span<const BufferBundle> globalUniformBufferBundles,
        std::span<const BufferBundle> lightingUniformBufferBundles) const noexcept;

    [[nodiscard]] Expected<void> drawShadowMapTarget(
        std::size_t imageIndex,
        std::span<const vk::CommandBuffer> commandBuffers,
        std::span<const BufferBundle> globalUniformBufferBundles) const noexcept;

private:
    Shared<e172vp::GraphicsObject> m_graphicsObject;

    Shared<e172vp::Pipeline> m_pipeline;
    Shared<e172vp::Pipeline> m_normalesPipeline;
    Shared<e172vp::Pipeline> m_shadowMapPipeline;

    UploadedMesh m_mesh;
    std::vector<UploadedMesh> m_debugMeshes;

    Shared<UploadedTexture> m_baseColorTexture;
    Shared<UploadedTexture> m_ambientOclussionMap;
    Shared<UploadedTexture> m_normalMap;

    std::vector<BadgerEngine::BufferBundle> m_uniformBufferBundles;
    std::vector<vk::DescriptorSet> m_baseColorTextureDescriptorSets;
    std::vector<vk::DescriptorSet> m_ambientOclussionMapDescriptorSets;
    std::vector<vk::DescriptorSet> m_normalMapDescriptorSets;
    std::vector<vk::DescriptorSet> m_shadowMapSamplerDescriptorSets;
};

}
