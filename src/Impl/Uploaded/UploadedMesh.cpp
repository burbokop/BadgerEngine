#include "UploadedMesh.h"

#include "../Buffers/BufferUtils.h"
#include "../graphicsobject.h"
#include <vulkan/vulkan.hpp>

namespace BadgerEngine {

struct UploadedMesh::Impl {
    struct Private {};

public:
    static Expected<Shared<Impl>> upload(Shared<e172vp::GraphicsObject> device, const Geometry::Mesh& mesh)
    {
        vk::Buffer vertexBuffer;
        vk::DeviceMemory vertexBufferMemory;
        vk::Buffer indexBuffer;
        vk::DeviceMemory indexBufferMemory;

        BufferUtils::createVertexBuffer(&*device, mesh.vertices(), &vertexBuffer, &vertexBufferMemory);
        BufferUtils::createIndexBuffer(&*device, mesh.indices(), &indexBuffer, &indexBufferMemory);

        return std::make_shared<Impl>(
            mesh.topology(),
            mesh.indices().size(),
            std::move(vertexBuffer),
            std::move(vertexBufferMemory),
            std::move(indexBuffer),
            std::move(indexBufferMemory),
            std::move(device),
            Private());
    }

    void bindTo(const vk::CommandBuffer& commandBuffer) const
    {
        vk::Buffer vb[] = { m_vertexBuffer };
        vk::DeviceSize offsets[] = { 0 };

        commandBuffer.bindVertexBuffers(0, 1, vb, offsets);
        commandBuffer.bindIndexBuffer(m_indexBuffer, 0, vk::IndexType::eUint32);
    }

    Geometry::Topology topology() const
    {
        return m_topology;
    }

    std::size_t indexCount() const
    {
        return m_indexCount;
    }

    Impl(
        Geometry::Topology topology,
        std::size_t indexCount,
        vk::Buffer&& vertexBuffer,
        vk::DeviceMemory&& vertexBufferMemory,
        vk::Buffer&& indexBuffer,
        vk::DeviceMemory&& indexBufferMemory,
        Shared<e172vp::GraphicsObject>&& device,
        Private)
        : m_topology(topology)
        , m_indexCount(indexCount)
        , m_vertexBuffer(std::move(vertexBuffer))
        , m_vertexBufferMemory(std::move(vertexBufferMemory))
        , m_indexBuffer(std::move(indexBuffer))
        , m_indexBufferMemory(std::move(indexBufferMemory))
        , m_device(std::move(device))
    {
    }

    ~Impl()
    {
        m_device->logicalDevice().destroyBuffer(m_vertexBuffer);
        m_device->logicalDevice().freeMemory(m_vertexBufferMemory);
        m_device->logicalDevice().destroyBuffer(m_indexBuffer);
        m_device->logicalDevice().freeMemory(m_indexBufferMemory);
    }

private:
    Geometry::Topology m_topology;
    std::size_t m_indexCount;
    vk::Buffer m_vertexBuffer;
    vk::DeviceMemory m_vertexBufferMemory;
    vk::Buffer m_indexBuffer;
    vk::DeviceMemory m_indexBufferMemory;
    Shared<e172vp::GraphicsObject> m_device;
};

Expected<UploadedMesh> UploadedMesh::upload(Shared<e172vp::GraphicsObject> device, UploadedMeshCache& cache, const Shared<Geometry::Mesh>& mesh)
{
    {
        const auto it = cache.m_cache.find(mesh.nullable());
        if (it != cache.m_cache.end()) {
            const auto key = it->first.lock();
            const auto value = it->second.lock();
            if (key && value) {
                return UploadedMesh(value);
            } else {
                cache.m_cache.erase(it);
            }
        }
    }

    auto result = Impl::upload(device, *mesh);
    if (!result) {
        return unexpected(result.error());
    }

    [[maybe_unused]] const auto ok = cache.m_cache.insert({ mesh.nullable(), result->nullable() }).second;
    assert(ok);

    return UploadedMesh(std::move(*result));
}

Geometry::Topology UploadedMesh::topology() const
{
    return m_impl->topology();
}

std::size_t UploadedMesh::indexCount() const
{
    return m_impl->indexCount();
}

void UploadedMesh::bindTo(const vk::CommandBuffer& commandBuffer) const
{
    m_impl->bindTo(commandBuffer);
}

}
