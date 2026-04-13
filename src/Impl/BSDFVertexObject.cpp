#include "BSDFVertexObject.h"

#include "../Geometry/Mesh.h"
#include "../RenderingOptions.h"
#include "../Utils/Collections.h"
#include "../Utils/NumericCast.h"
#include "Buffers/BufferUtils.h"
#include "Uploaded/UploadedTexture.h"
#include "graphicsobject.h"
#include "pipeline.h"
#include <list>

namespace BadgerEngine {

namespace {

struct UniformBufferObject {
    glm::mat4 model;
};

static_assert(offsetof(UniformBufferObject, model) == 0, "Offset must comply with std140");

std::vector<UploadedMesh> createDebugMeshes(
    Shared<e172vp::GraphicsObject> graphicsObject,
    const Geometry::Mesh& mesh,
    DisplayNormals displayNormals)
{
    std::list<UploadedMesh> result;

    const float len = 0.02f;

    UploadedMeshCache nopCache;

    switch (displayNormals) {
    case DisplayNormals::NoNormals:
        break;
    case DisplayNormals::VertexNormals:
        result.push_back(
            UploadedMesh::upload(graphicsObject, nopCache, mesh.vertexNormalsMesh(len, glm::vec3(0, 0, 1)).value()).transform_error(AsCritical()).value());
        result.push_back(
            UploadedMesh::upload(graphicsObject, nopCache, mesh.vertexTangentsMesh(len, glm::vec3(1, 0, 0)).value()).transform_error(AsCritical()).value());
        result.push_back(
            UploadedMesh::upload(graphicsObject, nopCache, mesh.vertexBitangentsMesh(len, glm::vec3(0, 1, 0)).value()).transform_error(AsCritical()).value());
        break;
    case DisplayNormals::PolygonNormals:
        result.push_back(
            UploadedMesh::upload(graphicsObject, nopCache, mesh.polygonNormalsMesh(len).value()).transform_error(AsCritical()).value());
        break;
    }

    return result | Collect<std::vector>;
}
}

BSDFVertexObject::BSDFVertexObject(
    Shared<e172vp::GraphicsObject> graphicsObject,
    std::size_t imageCount,
    const e172vp::DescriptorSetLayout& uniformBufferDescriptorSetLayout,
    const e172vp::DescriptorSetLayout& baseColorDescriptorSetLayout,
    const e172vp::DescriptorSetLayout& ambientOclussionDescriptorSetLayout,
    const e172vp::DescriptorSetLayout& normalMapDescriptorSetLayout,
    const e172vp::DescriptorSetLayout& shadowMapSamplerDescriptorSetLayout,
    const Shared<Geometry::Mesh>& mesh,
    UploadedMeshCache& cache,
    UploadedTexture baseColorTexture,
    UploadedTexture ambientOclussionMap,
    UploadedTexture normalMap,
    Shared<e172vp::Pipeline> pipeline,
    Shared<e172vp::Pipeline> normalesPipeline,
    Shared<e172vp::Pipeline> shadowMapPipeline,
    DisplayNormals displayNormals,
    bool castShadow)
    : m_graphicsObject(std::move(graphicsObject))
    , m_pipeline(std::move(pipeline))
    , m_normalesPipeline(std::move(normalesPipeline))
    , m_shadowMapPipeline(std::move(shadowMapPipeline))
    , m_mesh(UploadedMesh::upload(m_graphicsObject, cache, mesh).transform_error(AsCritical()).value())
    , m_debugMeshes(createDebugMeshes(m_graphicsObject, *mesh, displayNormals))
    , m_baseColorTexture(std::move(baseColorTexture))
    , m_ambientOclussionMap(std::move(ambientOclussionMap))
    , m_normalMap(std::move(normalMap))
    , m_castShadow(std::move(castShadow))
{
    m_uniformBufferBundles = BufferUtils::createUniformBufferBundle<UniformBufferObject>(
        *m_graphicsObject,
        m_graphicsObject->swapChain().imageCount(),
        m_graphicsObject->descriptorPool(),
        uniformBufferDescriptorSetLayout);

    BufferUtils::createSamplerDescriptorSets(
        m_graphicsObject->logicalDevice(),
        m_graphicsObject->descriptorPool(),
        m_baseColorTexture.view(),
        imageCount,
        m_graphicsObject->sampler(),
        baseColorDescriptorSetLayout,
        &m_baseColorTextureDescriptorSets,
        vk::ImageLayout::eShaderReadOnlyOptimal);

    BufferUtils::createSamplerDescriptorSets(
        m_graphicsObject->logicalDevice(),
        m_graphicsObject->descriptorPool(),
        m_ambientOclussionMap.view(),
        imageCount,
        m_graphicsObject->sampler(),
        ambientOclussionDescriptorSetLayout,
        &m_ambientOclussionMapDescriptorSets,
        vk::ImageLayout::eShaderReadOnlyOptimal);

    BufferUtils::createSamplerDescriptorSets(
        m_graphicsObject->logicalDevice(),
        m_graphicsObject->descriptorPool(),
        m_normalMap.view(),
        imageCount,
        m_graphicsObject->sampler(),
        normalMapDescriptorSetLayout,
        &m_normalMapDescriptorSets,
        vk::ImageLayout::eShaderReadOnlyOptimal);

    BufferUtils::createSamplerDescriptorSets(
        m_graphicsObject->logicalDevice(),
        m_graphicsObject->descriptorPool(),
        m_graphicsObject->swapChain().shadowMapImageViewVector(),
        m_graphicsObject->shadowSampler(),
        shadowMapSamplerDescriptorSetLayout,
        &m_shadowMapSamplerDescriptorSets,
        vk::ImageLayout::eDepthStencilReadOnlyOptimal);
}

Expected<void> BSDFVertexObject::draw(
    std::size_t imageIndex,
    std::span<const vk::CommandBuffer> commandBuffers,
    std::span<const BufferBundle> globalUniformBufferBundles,
    std::span<const BufferBundle> lightingUniformBufferBundles,
    RenderTarget target) const noexcept
{
    switch (target) {
    case RenderTarget::Color:
        return drawColorTarget(imageIndex, commandBuffers, globalUniformBufferBundles, lightingUniformBufferBundles);
    case RenderTarget::ShadowMap:
        return drawShadowMapTarget(imageIndex, commandBuffers, globalUniformBufferBundles);
    }

    std::unreachable();
}

Expected<void> BSDFVertexObject::drawColorTarget(
    std::size_t imageIndex,
    std::span<const vk::CommandBuffer> commandBuffers,
    std::span<const BufferBundle> globalUniformBufferBundles,
    std::span<const BufferBundle> lightingUniformBufferBundles) const noexcept
{
    m_pipeline->bindTo(commandBuffers[imageIndex]);
    m_mesh.bindTo(commandBuffers[imageIndex]);

    commandBuffers[imageIndex].bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        m_pipeline->pipelineLayout(),
        0,
        {
            globalUniformBufferBundles[imageIndex].descriptorSet,
            lightingUniformBufferBundles[imageIndex].descriptorSet,
            m_uniformBufferBundles.at(imageIndex).descriptorSet,
            m_baseColorTextureDescriptorSets.at(imageIndex),
            m_ambientOclussionMapDescriptorSets.at(imageIndex),
            m_normalMapDescriptorSets.at(imageIndex),
            m_shadowMapSamplerDescriptorSets.at(imageIndex),
        },
        {});

    commandBuffers[imageIndex].drawIndexed(numericCast<std::uint32_t>(m_mesh.indexCount()).value(), 1, 0, 0, 0);

    if (!m_debugMeshes.empty()) {
        m_normalesPipeline->bindTo(commandBuffers[imageIndex]);
    }

    for (const auto& mesh : m_debugMeshes) {
        mesh.bindTo(commandBuffers[imageIndex]);

        commandBuffers[imageIndex].bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_normalesPipeline->pipelineLayout(),
            0,
            {
                globalUniformBufferBundles[imageIndex].descriptorSet,
                lightingUniformBufferBundles[imageIndex].descriptorSet,
                m_uniformBufferBundles.at(imageIndex).descriptorSet,
            },
            {});

        commandBuffers[imageIndex].drawIndexed(numericCast<std::uint32_t>(mesh.indexCount()).value(), 1, 0, 0, 0);
    }

    return {};
}

Expected<void> BSDFVertexObject::drawShadowMapTarget(
    std::size_t imageIndex,
    std::span<const vk::CommandBuffer> commandBuffers,
    std::span<const BufferBundle> globalUniformBufferBundles) const noexcept
{
    if (!m_castShadow)
        return {};

    m_shadowMapPipeline->bindTo(commandBuffers[imageIndex]);
    m_mesh.bindTo(commandBuffers[imageIndex]);

    commandBuffers[imageIndex].bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        m_shadowMapPipeline->pipelineLayout(),
        0,
        {
            globalUniformBufferBundles[imageIndex].descriptorSet,
            m_uniformBufferBundles.at(imageIndex).descriptorSet,
        },
        {});

    commandBuffers[imageIndex].drawIndexed(numericCast<std::uint32_t>(m_mesh.indexCount()).value(), 1, 0, 0, 0);

    return {};
}

Expected<void> BSDFVertexObject::updateUniformBuffer(std::size_t imageIndex) noexcept
{
    const auto ubo = UniformBufferObject {
        .model = transformation() * translation() * rotation() * scale(),
    };

    void* data = nullptr;
    const auto result = m_graphicsObject->logicalDevice().mapMemory(m_uniformBufferBundles[imageIndex].memory, 0, sizeof(UniformBufferObject), vk::MemoryMapFlags(), &data);
    if (result != vk::Result::eSuccess) {
        return unexpected("Failed to map memory: " + vk::to_string(result));
    }
    assert(data);
    std::memcpy(data, &ubo, sizeof(UniformBufferObject));
    m_graphicsObject->logicalDevice().unmapMemory(m_uniformBufferBundles[imageIndex].memory);
    return {};
}

BSDFVertexObject::~BSDFVertexObject()
{
    for (size_t i = 0; i < m_uniformBufferBundles.size(); ++i) {
        m_graphicsObject->logicalDevice().destroyBuffer(m_uniformBufferBundles[i].buffer);
        m_graphicsObject->logicalDevice().freeMemory(m_uniformBufferBundles[i].memory);
        m_graphicsObject->logicalDevice().freeDescriptorSets(m_graphicsObject->descriptorPool(), m_uniformBufferBundles[i].descriptorSet);
    }
}

}
