#pragma once

#include "../Geometry/Topology.h"
#include "../Model/Model.h"
#include <vulkan/vulkan.hpp>

namespace e172vp {

class Pipeline final {
public:
    Pipeline(
        const vk::Device& logicalDevice,
        const vk::Extent2D& extent,
        const vk::RenderPass& renderPass,
        const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts,
        std::span<const std::uint8_t> vertexShader,
        std::span<const std::uint8_t> fragmentShader,
        BadgerEngine::Geometry::Topology topology,
        BadgerEngine::PolygonMode polygonMode,
        bool backfaceCulling);

    Pipeline(
        const vk::Device& logicalDevice,
        const vk::Extent2D& extent,
        const vk::RenderPass& renderPass,
        const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts,
        std::span<const std::uint8_t> vertexShader,
        BadgerEngine::Geometry::Topology topology,
        BadgerEngine::PolygonMode polygonMode, bool backfaceCulling);

    Pipeline() = delete;
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline(Pipeline&&) = delete;
    Pipeline& operator=(Pipeline&&) = delete;

    vk::Pipeline handle() const;
    vk::PipelineLayout pipelineLayout() const;
    vk::Device logicalDevice() const;

    BadgerEngine::Geometry::Topology topology()
    {
        return m_topology;
    }

    void bindTo(const vk::CommandBuffer& commandBuffer) const
    {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_handle);
    }

    ~Pipeline();

private:
    static vk::ShaderModule createShaderModule(const vk::Device& logicDevice, std::span<const std::uint8_t> code);

private:
    vk::Pipeline m_handle;
    vk::PipelineLayout m_pipelineLayout;
    vk::Device m_logicalDevice;
    BadgerEngine::Geometry::Topology m_topology;
};
}
