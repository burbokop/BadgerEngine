#include "MeshBuffer.h"

#include "../graphicsobject.h"
#include "BufferUtils.h"
#include <vulkan/vulkan.hpp>

namespace BadgerEngine {

struct MeshBuffer::Impl {
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

Expected<MeshBuffer> MeshBuffer::upload(Shared<e172vp::GraphicsObject> device, const Geometry::Mesh& mesh)
{
    auto r = Impl::upload(device, mesh);
    if (!r) {
        return unexpected(r.error());
    }
    return MeshBuffer(std::move(*r));
}

Geometry::Topology MeshBuffer::topology() const
{
    return m_impl->topology();
}

std::size_t MeshBuffer::indexCount() const
{
    return m_impl->indexCount();
}

void MeshBuffer::bindTo(const vk::CommandBuffer& commandBuffer) const
{
    m_impl->bindTo(commandBuffer);
}
}
