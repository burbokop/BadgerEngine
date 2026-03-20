#include "BSDFVertexObject.h"

#include "Buffers/BufferUtils.h"
#include "Geometry/Mesh.h"
#include "RenderingOptions.h"
#include "Tools/UploadedTexture.h"
#include "Utils/Collections.h"
#include "Utils/NumericCast.h"
#include "graphicsobject.h"
#include "pipeline.h"
#include <list>

namespace BadgerEngine {

namespace {

struct UniformBufferObject {
    glm::mat4 model;
};

static_assert(offsetof(UniformBufferObject, model) == 0, "Offset must comply with std140");

std::vector<UploadedModel> createModels(
    Shared<e172vp::GraphicsObject> graphicsObject,
    const Shared<Geometry::Mesh>& mesh,
    Shared<e172vp::Pipeline> pipeline,
    Shared<e172vp::Pipeline> nPipeline,
    DisplayNormals displayNormals)
{
    std::list<UploadedModel> result = {
        UploadedModel(
            MeshBuffer::upload(graphicsObject, *mesh).transform_error(AsCritical()).value(),
            std::move(pipeline))
    };

    const float len = 0.02f;

    switch (displayNormals) {
    case DisplayNormals::NoNormals:
        break;
    case DisplayNormals::VertexNormals:
        result.push_back(UploadedModel(
            MeshBuffer::upload(graphicsObject, *mesh->vertexNormalsMesh(len, glm::vec3(0, 0, 1)).value()).transform_error(AsCritical()).value(),
            nPipeline));
        result.push_back(UploadedModel(
            MeshBuffer::upload(graphicsObject, *mesh->vertexTangentsMesh(len, glm::vec3(1, 0, 0)).value()).transform_error(AsCritical()).value(),
            nPipeline));
        result.push_back(UploadedModel(
            MeshBuffer::upload(graphicsObject, *mesh->vertexBitangentsMesh(len, glm::vec3(0, 1, 0)).value()).transform_error(AsCritical()).value(),
            std::move(nPipeline)));
        break;
    case DisplayNormals::PolygonNormals:
        result.push_back(UploadedModel(
            MeshBuffer::upload(graphicsObject, *mesh->polygonNormalsMesh(len).value()).transform_error(AsCritical()).value(),
            std::move(nPipeline)));
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
    const Shared<Geometry::Mesh>& mesh,
    Shared<UploadedTexture> baseColorTexture,
    Shared<UploadedTexture> ambientOclussionMap,
    Shared<UploadedTexture> normalMap,
    Shared<e172vp::Pipeline> pipeline,
    Shared<e172vp::Pipeline> nPipeline,
    DisplayNormals displayNormals)
    : m_graphicsObject(std::move(graphicsObject))
    , m_models(createModels(m_graphicsObject, mesh, std::move(pipeline), std::move(nPipeline), displayNormals))
    , m_baseColorTexture(std::move(baseColorTexture))
    , m_ambientOclussionMap(std::move(ambientOclussionMap))
    , m_normalMap(std::move(normalMap))
{
    m_uniformBufferBundles = BufferUtils::createUniformBufferBundle<UniformBufferObject>(
        *m_graphicsObject,
        m_graphicsObject->swapChain().imageCount(),
        m_graphicsObject->descriptorPool(),
        uniformBufferDescriptorSetLayout);

    BufferUtils::createSamplerDescriptorSets(
        m_graphicsObject->logicalDevice(),
        m_graphicsObject->descriptorPool(),
        m_baseColorTexture->imageView(),
        m_graphicsObject->sampler(),
        imageCount,
        baseColorDescriptorSetLayout,
        &m_baseColorTextureDescriptorSets);

    BufferUtils::createSamplerDescriptorSets(
        m_graphicsObject->logicalDevice(),
        m_graphicsObject->descriptorPool(),
        m_ambientOclussionMap->imageView(),
        m_graphicsObject->sampler(),
        imageCount,
        ambientOclussionDescriptorSetLayout,
        &m_ambientOclussionMapDescriptorSets);

    BufferUtils::createSamplerDescriptorSets(
        m_graphicsObject->logicalDevice(),
        m_graphicsObject->descriptorPool(),
        m_normalMap->imageView(),
        m_graphicsObject->sampler(),
        imageCount,
        normalMapDescriptorSetLayout,
        &m_normalMapDescriptorSets);
}

Expected<void> BSDFVertexObject::draw(
    std::size_t imageIndex,
    std::span<const vk::CommandBuffer> commandBuffers,
    std::span<const BufferBundle> commonGlobalUniformBufferBundles,
    std::span<const BufferBundle> lightingUniformBufferBundles) const noexcept
{
    for (const auto& model : m_models) {
        model.bindTo(commandBuffers[imageIndex]);

        commandBuffers[imageIndex].bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            model.pipeline()->pipelineLayout(),
            0,
            {
                commonGlobalUniformBufferBundles[imageIndex].descriptorSet,
                lightingUniformBufferBundles[imageIndex].descriptorSet,
                m_uniformBufferBundles.at(imageIndex).descriptorSet,
                m_baseColorTextureDescriptorSets.at(imageIndex),
                m_ambientOclussionMapDescriptorSets.at(imageIndex),
                m_normalMapDescriptorSets.at(imageIndex),
            },
            {});

        commandBuffers[imageIndex].drawIndexed(numericCast<std::uint32_t>(model.indexCount()).value(), 1, 0, 0, 0);
    }

    return {};
}

Expected<void> BSDFVertexObject::updateUniformBuffer(std::size_t imageIndex) noexcept
{
    const auto ubo = UniformBufferObject {
        .model = translation() * rotation() * scale(),
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
