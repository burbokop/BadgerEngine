#include "pipeline.h"

#include "../Geometry/Vertex.h"
#include "../Utils/NumericCast.h"

namespace {

[[nodiscard]] vk::PolygonMode polygonModeToVk(BadgerEngine::PolygonMode pm) noexcept
{
    switch (pm) {
    case BadgerEngine::PolygonMode::Fill:
        return vk::PolygonMode::eFill;
    case BadgerEngine::PolygonMode::Line:
        return vk::PolygonMode::eLine;
    case BadgerEngine::PolygonMode::Point:
        return vk::PolygonMode::ePoint;
    case BadgerEngine::PolygonMode::FillRectangleNV:
        return vk::PolygonMode::eFillRectangleNV;
    }
    std::unreachable();
}

vk::PrimitiveTopology vulkanPrimitiveTopologyFromTopology(BadgerEngine::Geometry::Topology topology)
{
    switch (topology) {
    case BadgerEngine::Geometry::Topology::LineList:
        return vk::PrimitiveTopology::eLineList;
    case BadgerEngine::Geometry::Topology::TriangleList:
        return vk::PrimitiveTopology::eTriangleList;
    }
    std::abort();
}

}

vk::Pipeline e172vp::Pipeline::handle() const
{
    return m_handle;
}

vk::PipelineLayout e172vp::Pipeline::pipelineLayout() const
{
    return m_pipelineLayout;
}

e172vp::Pipeline::~Pipeline()
{
    if (m_logicalDevice) {
        if (m_pipelineLayout)
            m_logicalDevice.destroyPipelineLayout(m_pipelineLayout);
        if (m_handle)
            m_logicalDevice.destroyPipeline(m_handle);
    }
}

vk::Device e172vp::Pipeline::logicalDevice() const
{
    return m_logicalDevice;
}

vk::ShaderModule e172vp::Pipeline::createShaderModule(const vk::Device& logicDevice, std::span<const std::uint8_t> code)
{
    assert(!code.empty());
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.setCodeSize(code.size());
    createInfo.setPCode(reinterpret_cast<const std::uint32_t*>(code.data()));

    vk::ShaderModule shaderModule;
    if (logicDevice.createShaderModule(&createInfo, nullptr, &shaderModule) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

e172vp::Pipeline::Pipeline(
    const vk::Device& logicalDevice,
    const vk::Extent2D& extent,
    const vk::RenderPass& renderPass,
    const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts,
    std::span<const std::uint8_t> vertexShader,
    std::span<const std::uint8_t> fragmentShader,
    BadgerEngine::Geometry::Topology topology,
    BadgerEngine::PolygonMode polygonMode,
    bool backfaceCulling)
    : m_topology(topology)
{
    vk::ShaderModule vertShaderModule = createShaderModule(logicalDevice, vertexShader);
    vk::ShaderModule fragShaderModule = createShaderModule(logicalDevice, fragmentShader);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo {};
    vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo {};
    fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    // vertex input
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo {};
    const auto bindingDescription = BadgerEngine::Geometry::Vertex::bindingDescription();
    const auto attributeDescriptions = BadgerEngine::Geometry::Vertex::attributeDescriptions();
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly {};
    inputAssembly.topology = vulkanPrimitiveTopologyFromTopology(topology);
    inputAssembly.primitiveRestartEnable = false;

    vk::Viewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vk::Rect2D scissor;
    scissor.offset = vk::Offset2D(0, 0);
    scissor.extent = extent;

    vk::PipelineViewportStateCreateInfo viewportState;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    vk::PipelineRasterizationStateCreateInfo rasterizer {};
    rasterizer.depthClampEnable = false;
    rasterizer.rasterizerDiscardEnable = false;
    rasterizer.polygonMode = polygonModeToVk(polygonMode);
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = backfaceCulling ? vk::CullModeFlagBits::eBack : vk::CullModeFlagBits::eNone;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = false;

    vk::PipelineDepthStencilStateCreateInfo depthStencilState;
    depthStencilState.flags = vk::PipelineDepthStencilStateCreateFlags();
    depthStencilState.depthTestEnable = true;
    depthStencilState.depthWriteEnable = true;
    depthStencilState.depthCompareOp = vk::CompareOp::eLess;
    depthStencilState.depthBoundsTestEnable = false;
    depthStencilState.stencilTestEnable = false;

    vk::PipelineMultisampleStateCreateInfo multisampling;
    multisampling.sampleShadingEnable = false;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment {};
    colorBlendAttachment.colorWriteMask
        = vk::ColorComponentFlagBits::eR
        | vk::ColorComponentFlagBits::eG
        | vk::ColorComponentFlagBits::eB
        | vk::ColorComponentFlagBits::eA;

    colorBlendAttachment.blendEnable = false;

    vk::PipelineColorBlendStateCreateInfo colorBlending {};
    colorBlending.logicOpEnable = false;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo {};
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    pipelineLayoutInfo.setLayoutCount = BadgerEngine::numericCast<std::uint32_t>(descriptorSetLayouts.size()).value();
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

    if (logicalDevice.createPipelineLayout(&pipelineLayoutInfo, nullptr, &m_pipelineLayout) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    vk::GraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.pDepthStencilState = &depthStencilState;

    if (logicalDevice.createGraphicsPipelines(vk::PipelineCache(), 1, &pipelineInfo, nullptr, &m_handle) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    logicalDevice.destroyShaderModule(fragShaderModule);
    logicalDevice.destroyShaderModule(vertShaderModule);
    m_logicalDevice = logicalDevice;
}

e172vp::Pipeline::Pipeline(
    const vk::Device& logicalDevice,
    const vk::Extent2D& extent,
    const vk::RenderPass& renderPass,
    const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts,
    std::span<const uint8_t> vertexShader,
    BadgerEngine::Geometry::Topology topology,
    BadgerEngine::PolygonMode polygonMode,
    bool backfaceCulling)
{
    vk::ShaderModule vertShaderModule = createShaderModule(logicalDevice, vertexShader);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo {};
    vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    std::array<vk::PipelineShaderStageCreateInfo, 1> shaderStages = { vertShaderStageInfo };

    // vertex input
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo {};
    const auto bindingDescription = BadgerEngine::Geometry::Vertex::bindingDescription();
    const auto attributeDescriptions = BadgerEngine::Geometry::Vertex::attributeDescriptions();
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly {};
    inputAssembly.topology = vulkanPrimitiveTopologyFromTopology(topology);
    inputAssembly.primitiveRestartEnable = false;

    const vk::Viewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    const vk::Rect2D scissor = {
        .offset = vk::Offset2D(0, 0),
        .extent = extent,
    };

    const vk::PipelineViewportStateCreateInfo viewportState = {
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };

    const vk::PipelineRasterizationStateCreateInfo rasterizer = {
        .depthClampEnable = false,
        .rasterizerDiscardEnable = false,
        .polygonMode = polygonModeToVk(polygonMode),
        .cullMode = backfaceCulling ? vk::CullModeFlagBits::eBack : vk::CullModeFlagBits::eNone,
        .frontFace = vk::FrontFace::eClockwise,
        .depthBiasEnable = true,
        .lineWidth = 1.0f,
    };

    const vk::PipelineDepthStencilStateCreateInfo depthStencilState = {
        .flags = vk::PipelineDepthStencilStateCreateFlags(),
        .depthTestEnable = true,
        .depthWriteEnable = true,
        .depthCompareOp = vk::CompareOp::eLessOrEqual,
        .depthBoundsTestEnable = false,
        .stencilTestEnable = false,
    };

    const vk::PipelineMultisampleStateCreateInfo multisampling = {
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = false,
    };

    const vk::PipelineColorBlendStateCreateInfo colorBlending = {
        .logicOpEnable = false,
        .logicOp = vk::LogicOp::eCopy,
        .attachmentCount = 0,
        .pAttachments = nullptr,
        .blendConstants = std::array { 0.0f, 0.0f, 0.0f, 0.0f },
    };

    const vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {
        .setLayoutCount = BadgerEngine::numericCast<std::uint32_t>(descriptorSetLayouts.size()).value(),
        .pSetLayouts = descriptorSetLayouts.data(),
        .pushConstantRangeCount = 0,
    };

    if (logicalDevice.createPipelineLayout(&pipelineLayoutInfo, nullptr, &m_pipelineLayout) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    const vk::GraphicsPipelineCreateInfo pipelineInfo = {
        .stageCount = shaderStages.size(),
        .pStages = shaderStages.data(),
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depthStencilState,
        .pColorBlendState = &colorBlending,
        .layout = m_pipelineLayout,
        .renderPass = renderPass,
        .subpass = 0,
    };

    if (logicalDevice.createGraphicsPipelines(vk::PipelineCache(), 1, &pipelineInfo, nullptr, &m_handle) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    logicalDevice.destroyShaderModule(vertShaderModule);
    m_logicalDevice = logicalDevice;
}
