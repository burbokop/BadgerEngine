#pragma once

#include "../Geometry/Vertex.h"
#include "../descriptorsetlayout.h"
#include <vulkan/vulkan.hpp>

namespace e172vp {
class GraphicsObject;
}

namespace BadgerEngine {

struct BufferBundle {
    vk::Buffer buffer;
    vk::DeviceMemory memory;
    vk::DescriptorSet descriptorSet;
};

class Buffer {
    static vk::Device logicalDevice(const e172vp::GraphicsObject* graphicsObject);
    static vk::PhysicalDevice physicalDevice(const e172vp::GraphicsObject* graphicsObject);
    static inline const auto uniformBufferType = vk::BufferUsageFlagBits::eUniformBuffer;
    static inline const auto descriptorType = vk::DescriptorType::eUniformBufferDynamic;
public:
    static uint32_t findMemoryType(const vk::PhysicalDevice &physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    static bool createAbstractBuffer(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer *buffer, vk::DeviceMemory *bufferMemory);
    static void copyBuffer(const vk::Device &logicalDevice, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const vk::Buffer &srcBuffer, const vk::Buffer &dstBuffer, const vk::DeviceSize &size);

    static void createVertexBuffer(
        const vk::Device& logicalDevice,
        const vk::PhysicalDevice& physicalDevice,
        const vk::CommandPool& commandPool,
        const vk::Queue& graphicsQueue,
        const std::vector<BadgerEngine::Geometry::Vertex>& vertices,
        vk::Buffer* vertexBuffer,
        vk::DeviceMemory* vertexBufferMemory);

    static void createIndexBuffer(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const std::vector<uint32_t> &indices, vk::Buffer *indexBuffer, vk::DeviceMemory *indexBufferMemory);

    static void createAbstractBuffer(const e172vp::GraphicsObject* graphicsObject, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer* buffer, vk::DeviceMemory* bufferMemory);
    static void copyBuffer(const e172vp::GraphicsObject* graphicsObject, const vk::Buffer& srcBuffer, const vk::Buffer& dstBuffer, const vk::DeviceSize& size);
    static void createVertexBuffer(
        const e172vp::GraphicsObject* graphicsObject,
        const std::vector<BadgerEngine::Geometry::Vertex>& vertices,
        vk::Buffer* vertexBuffer,
        vk::DeviceMemory* vertexBufferMemory);

    static void createIndexBuffer(const e172vp::GraphicsObject* graphicsObject, const std::vector<uint32_t>& indices, vk::Buffer* indexBuffer, vk::DeviceMemory* indexBufferMemory);

    template<typename T>
    static void createUniformBuffer(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, vk::Buffer *uniformBuffers, vk::DeviceMemory *uniformBuffersMemory) {
        createAbstractBuffer(logicalDevice, physicalDevice, sizeof(T), uniformBufferType, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformBuffers, uniformBuffersMemory);
    }

    template<typename T>
    static void createUniformBuffer(const e172vp::GraphicsObject* graphicsObject, vk::Buffer* uniformBuffers, vk::DeviceMemory* uniformBuffersMemory)
    {
        createAbstractBuffer(logicalDevice(graphicsObject), physicalDevice(graphicsObject), sizeof(T), uniformBufferType, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformBuffers, uniformBuffersMemory);
    }

    template<typename T>
    static void createUniformBuffers(const e172vp::GraphicsObject* graphicsObject, size_t count, std::vector<vk::Buffer>* uniformBuffers, std::vector<vk::DeviceMemory>* uniformBuffersMemory)
    {
        uniformBuffers->resize(count);
        uniformBuffersMemory->resize(count);

        for(size_t i = 0; i < count; ++i) {
            Buffer::createUniformBuffer<T>(
                        logicalDevice(graphicsObject),
                        physicalDevice(graphicsObject),
                        &uniformBuffers->operator[](i),
                        &uniformBuffersMemory->operator[](i)
                        );
        }
    }

    template<typename T>
    static void createUniformBuffers(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, size_t count, std::vector<vk::Buffer> *uniformBuffers, std::vector<vk::DeviceMemory> *uniformBuffersMemory) {
        uniformBuffers->resize(count);
        uniformBuffersMemory->resize(count);

        for(size_t i = 0; i < count; ++i) {
            Buffer::createUniformBuffer<T>(
                        logicalDevice,
                        physicalDevice,
                        &uniformBuffers->operator[](i),
                        &uniformBuffersMemory->operator[](i)
                        );
        }
    }

    static void createUniformDescriptorSets(
        const vk::Device& logicalDevice,
        const vk::DescriptorPool& descriptorPool,
        size_t structSize,
        const std::vector<vk::Buffer>& uniformBuffers,
        const e172vp::DescriptorSetLayout* descriptorSetLayout,
        std::vector<vk::DescriptorSet>* descriptorSets);

    template<typename T>
    static std::vector<BufferBundle> createUniformBufferBundle(
        const e172vp::GraphicsObject& graphicsObject,
        std::size_t count,
        const vk::Device& logicalDevice,
        const vk::DescriptorPool& descriptorPool,
        const e172vp::DescriptorSetLayout& descriptorSetLayout)
    {
        std::vector<vk::Buffer> buffers;
        std::vector<vk::DeviceMemory> bufferMemories;
        std::vector<vk::DescriptorSet> descriptorSets;

        createUniformBuffers<T>(
            &graphicsObject,
            count,
            &buffers,
            &bufferMemories);

        createUniformDescriptorSets<T>(
            logicalDevice,
            descriptorPool,
            buffers,
            &descriptorSetLayout,
            &descriptorSets);

        assert(buffers.size() == bufferMemories.size() && buffers.size() == descriptorSets.size());

        std::vector<BufferBundle> result(buffers.size());
        for (std::size_t i = 0; i < buffers.size(); ++i) {
            result[i] = {
                .buffer = buffers[i],
                .memory = bufferMemories[i],
                .descriptorSet = descriptorSets[i]
            };
        }
        return result;
    }

    static void createSamplerDescriptorSets(const vk::Device& logicalDevice, const vk::DescriptorPool& descriptorPool, const vk::ImageView& imageView, const vk::Sampler& sampler, size_t count, const e172vp::DescriptorSetLayout* descriptorSetLayout, std::vector<vk::DescriptorSet>* descriptorSets);

    template<typename T>
    static void createUniformDescriptorSets(const vk::Device &logicalDevice, const vk::DescriptorPool &descriptorPool, const std::vector<vk::Buffer> &uniformBuffers, const e172vp::DescriptorSetLayout *descriptorSetLayout, std::vector<vk::DescriptorSet> *descriptorSets) {
        createUniformDescriptorSets(logicalDevice, descriptorPool, sizeof (T), uniformBuffers, descriptorSetLayout, descriptorSets);
    }
};

}
