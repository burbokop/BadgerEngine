#include "vertexobject.h"

#include "Buffers/BufferUtils.h"
#include "Geometry/Mesh.h"
#include "Utils/Collections.h"
#include "graphicsobject.h"
#include "pipeline.h"

#include <list>

namespace BadgerEngine {

namespace {

struct UniformBufferObject {
    glm::mat4 model;
};

std::vector<UploadedModel> createModels(Shared<e172vp::GraphicsObject> graphicsObject, const BadgerEngine::Geometry::Mesh& mesh, Shared<e172vp::Pipeline> pipeline, Shared<e172vp::Pipeline> nPipeline)
{
    std::list<UploadedModel> result = {
        UploadedModel(
            MeshBuffer::upload(graphicsObject, mesh).value(),
            std::move(pipeline))
    };

    enum Normals {
        NoNormals,
        VertexNormals,
        PolygonNormals
    };

    const Normals displayNormals = PolygonNormals;
    const float len = 1;

    switch (displayNormals) {
    case NoNormals:
        break;
    case VertexNormals:
        result.push_back(UploadedModel(
            MeshBuffer::upload(graphicsObject, mesh.vertexNormalsMesh(len).value()).value(),
            std::move(nPipeline)));
        break;
    case PolygonNormals:
        result.push_back(UploadedModel(
            MeshBuffer::upload(graphicsObject, mesh.polygonNormalsMesh(len).value()).value(),
            std::move(nPipeline)));
        break;
    }

    return result | Collect<std::vector>;
}
}

VertexObject::VertexObject(Shared<e172vp::GraphicsObject> graphicsObject,
    size_t imageCount,
    const e172vp::DescriptorSetLayout* descriptorSetLayout,
    const e172vp::DescriptorSetLayout* samplerDescriptorSetLayout,
    const BadgerEngine::Geometry::Mesh& mesh,
    const vk::ImageView& imageView,
    Shared<e172vp::Pipeline> pipeline, Shared<e172vp::Pipeline> nPipeline)
    : m_graphicsObject(std::move(graphicsObject))
    , m_models(createModels(m_graphicsObject, mesh, std::move(pipeline), std::move(nPipeline)))
{
    m_uniformBufferBundles = BufferUtils::createUniformBufferBundle<UniformBufferObject>(
        *m_graphicsObject,
        m_graphicsObject->swapChain().imageCount(),
        m_graphicsObject->descriptorPool(),
        *descriptorSetLayout);

    BufferUtils::createSamplerDescriptorSets(m_graphicsObject->logicalDevice(),
        m_graphicsObject->descriptorPool(),
        imageView,
        m_graphicsObject->sampler(),
        imageCount,
        samplerDescriptorSetLayout,
        &m_textureDescriptorSets);
}

void VertexObject::updateUbo(int imageIndex)
{

    UniformBufferObject __ubo;
    __ubo.model = m_translation * m_rotation * m_scale;

    void* data;
    vkMapMemory(m_graphicsObject->logicalDevice(), m_uniformBufferBundles[imageIndex].memory, 0, sizeof(UniformBufferObject), 0, &data);
    assert(data);
    memcpy(data, &__ubo, sizeof(UniformBufferObject));
    vkUnmapMemory(m_graphicsObject->logicalDevice(), m_uniformBufferBundles[imageIndex].memory);
}

glm::mat4 VertexObject::rotation() const
{
    return m_rotation;
}

void VertexObject::setRotation(const glm::mat4& rotation)
{
    m_rotation = rotation;
}

glm::mat4 VertexObject::translation() const
{
    return m_translation;
}

void VertexObject::setTranslation(const glm::mat4& translation)
{
    m_translation = translation;
}

glm::mat4 VertexObject::scale() const
{
    return m_scale;
}

void VertexObject::setScale(const glm::mat4& scale)
{
    m_scale = scale;
}

std::vector<vk::DescriptorSet> VertexObject::textureDescriptorSets() const
{
    return m_textureDescriptorSets;
}

void VertexObject::draw(std::size_t imageIndex,
    std::span<const vk::CommandBuffer> commandBuffers,
    std::span<const BufferBundle> commonGlobalUniformBufferBundles,
    std::span<const BufferBundle> lightingUniformBufferBundles) const
{
    for (const auto& model : m_models) {
        model.bindTo(commandBuffers[imageIndex]);

        commandBuffers[imageIndex].bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            model.pipeline()->pipelineLayout(),
            0,
            { commonGlobalUniformBufferBundles[imageIndex].descriptorSet,
                lightingUniformBufferBundles[imageIndex].descriptorSet,
                bufferBundles()[imageIndex].descriptorSet,
                textureDescriptorSets()[imageIndex] },
            {});

        commandBuffers[imageIndex].drawIndexed(model.indexCount(), 1, 0, 0, 0);
    }
}

VertexObject::~VertexObject()
{
    for (size_t i = 0; i < m_uniformBufferBundles.size(); ++i) {
        m_graphicsObject->logicalDevice().destroyBuffer(m_uniformBufferBundles[i].buffer);
        m_graphicsObject->logicalDevice().freeMemory(m_uniformBufferBundles[i].memory);
        m_graphicsObject->logicalDevice().freeDescriptorSets(m_graphicsObject->descriptorPool(), m_uniformBufferBundles[i].descriptorSet);
    }
}

}
