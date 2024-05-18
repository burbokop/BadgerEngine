#include "renderer.h"

#include "Buffers/BufferUtils.h"
#include "Camera.h"
#include "Tools/Model.h"
#include "Tools/stringvector.h"
#include "Window.h"
#include <chrono>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <math.h>
#include <normal_debug.frag.spv.h>
#include <normal_debug.vert.spv.h>

namespace BadgerEngine {

namespace {

struct PointLightUniformBufferObject {
    glm::vec4 position;
    glm::vec4 color;
};

struct LightingUniformBufferObject {
    PointLightUniformBufferObject lights[64];
};

struct GlobalUniformBufferObject {
    glm::mat4 transformation;
    float time;
    glm::vec2 mouse;
};
}

Renderer::Renderer(Shared<Window> window)
    : m_graphicsObject(std::make_shared<e172vp::GraphicsObject>([window] {
        e172vp::GraphicsObjectCreateInfo createInfo;
        createInfo.setRequiredExtensions(window->requiredVulkanExtensions());
        createInfo.setApplicationName("test-app");
        createInfo.setApplicationVersion(1);
#ifndef NDEBUG
        createInfo.setDebugEnabled(true);
#endif
        createInfo.setRequiredDeviceExtensions({ VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_EXT_memory_budget" });
        createInfo.setSurfaceCreator([window](vk::Instance i, vk::SurfaceKHR* s) {
            *s = window->createVulkanSurface(i).value();
        });
        return createInfo;
    }()))
    , m_window(std::move(window))
    , m_camera(std::make_shared<PerspectiveCamera>())
{

    if (m_graphicsObject->debugEnabled())
        std::cout << "Used validation layers: " << e172vp::StringVector::toString(m_graphicsObject->enabledValidationLayers()) << "\n";

    if (!m_graphicsObject->isValid())
        std::cout << "GRAPHICS OBJECT IS NOT CREATED BECAUSE OF FOLOWING ERRORS:\n\n";

    const auto errors = m_graphicsObject->pullErrors();
    if (errors.size() > 0) {
        std::cerr << e172vp::StringVector::toString(errors) << "\n";
    }

    m_globalDescriptorSetLayout = e172vp::DescriptorSetLayout::createUniformDSL(m_graphicsObject->logicalDevice(), 0);
    m_lightingDescriptorSetLayout = e172vp::DescriptorSetLayout::createUniformDSL(m_graphicsObject->logicalDevice(), 0);
    m_objectDescriptorSetLayout = e172vp::DescriptorSetLayout::createUniformDSL(m_graphicsObject->logicalDevice(), 0);
    m_samplerDescriptorSetLayout = e172vp::DescriptorSetLayout::createSamplerDSL(m_graphicsObject->logicalDevice(), 0);

    m_commonGlobalUniformBufferBundles = BufferUtils::createUniformBufferBundle<GlobalUniformBufferObject>(
        *m_graphicsObject,
        m_graphicsObject->swapChain().imageCount(),
        m_graphicsObject->descriptorPool(),
        m_globalDescriptorSetLayout);

    m_lightingUniformBufferBundles = BufferUtils::createUniformBufferBundle<GlobalUniformBufferObject>(
        *m_graphicsObject,
        m_graphicsObject->swapChain().imageCount(),
        m_graphicsObject->descriptorPool(),
        m_lightingDescriptorSetLayout);

    //    bool useUniformBuffer = true;
    //    std::vector<char> vertShaderCode;
    //    if(useUniformBuffer) {
    //        vertShaderCode = readFile("shaders/vert_uniform.vert.spv");
    //    } else {
    //        vertShaderCode = readFile("shaders/shader.vert.spv");
    //    }
    //    std::vector<char> fragShaderCode = readFile("shaders/shader.frag.spv");

    createSyncObjects(m_graphicsObject->logicalDevice(), &m_imageAvailableSemaphore, &m_renderFinishedSemaphore);

    m_font = new e172vp::Font(m_graphicsObject->logicalDevice(),
        m_graphicsObject->physicalDevice(),
        m_graphicsObject->commandPool(),
        m_graphicsObject->graphicsQueue(),
        "fonts/ZCOOL.ttf",
        128);

    m_normalDebugPipeline = createPipeline(
        std::vector<std::uint8_t>(normal_debug_vert, normal_debug_vert + sizeof(normal_debug_vert) / sizeof(normal_debug_vert[0])),
        std::vector<std::uint8_t>(normal_debug_frag, normal_debug_frag + sizeof(normal_debug_frag) / sizeof(normal_debug_frag[0])),
        Geometry::Topology::LineList);
}

std::shared_ptr<e172vp::Pipeline> Renderer::createPipeline(
    const std::vector<std::uint8_t>& vertShaderCode,
    const std::vector<std::uint8_t>& fragShaderCode,
    Geometry::Topology topology)
{
    return std::make_shared<e172vp::Pipeline>(m_graphicsObject->logicalDevice(),
        m_graphicsObject->swapChainSettings().extent,
        m_graphicsObject->renderPass(),
        std::vector {
            m_globalDescriptorSetLayout.descriptorSetLayoutHandle(),
            m_lightingDescriptorSetLayout.descriptorSetLayoutHandle(),
            m_objectDescriptorSetLayout.descriptorSetLayoutHandle(),
            m_samplerDescriptorSetLayout.descriptorSetLayoutHandle() },
        vertShaderCode,
        fragShaderCode,
        topology);
}

void Renderer::applyPresentation()
{
    resetCommandBuffers(
        m_graphicsObject->commandPool().commandBufferVector(),
        m_graphicsObject->graphicsQueue(),
        m_graphicsObject->presentQueue());

    proceedCommandBuffers(
        m_graphicsObject->renderPass(),
        m_graphicsObject->swapChainSettings().extent,
        *m_camera,
        m_graphicsObject->renderPass().frameBufferVector(),
        m_graphicsObject->commandPool().commandBufferVector(),
        m_commonGlobalUniformBufferBundles,
        m_lightingUniformBufferBundles,
        m_vertexObjects);

    std::uint32_t imageIndex = 0;
    vk::Result returnCode;

    returnCode = m_graphicsObject->logicalDevice().acquireNextImageKHR(m_graphicsObject->swapChain(), UINT64_MAX, m_imageAvailableSemaphore, {}, &imageIndex);
    if(returnCode != vk::Result::eSuccess)
        throw std::runtime_error("acquiring next image failed. code: " + vk::to_string(returnCode));

    auto currentImageCommandBuffer = m_graphicsObject->commandPool().commandBuffer(imageIndex);

    vk::Semaphore waitSemaphores[] = { m_imageAvailableSemaphore };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };


    updateUniformBuffer(imageIndex);

    for (auto& o : m_vertexObjects) {
        o->updateUbo(imageIndex);
    }

    vk::SubmitInfo submitInfo;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.setCommandBuffers(currentImageCommandBuffer);
    submitInfo.setSignalSemaphores(m_renderFinishedSemaphore);

    returnCode = m_graphicsObject->graphicsQueue().submit(1, &submitInfo, {});

    if (returnCode != vk::Result::eSuccess)
        throw std::runtime_error("failed to submit draw command buffer. code: " + vk::to_string(returnCode));

    vk::SwapchainKHR swapChains[] = { m_graphicsObject->swapChain() };

    vk::PresentInfoKHR presentInfo;
    presentInfo.setWaitSemaphores(m_renderFinishedSemaphore);
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    returnCode = m_graphicsObject->presentQueue().presentKHR(&presentInfo);
    if(returnCode != vk::Result::eSuccess)
        throw std::runtime_error("present failed. code: " + vk::to_string(returnCode));
}

void Renderer::updateUniformBuffer(uint32_t currentImage)
{
    static const auto begin = std::chrono::high_resolution_clock::now();
    const float time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin).count() / 1000.f;

    const auto size = m_window->size();

    const auto ubo = GlobalUniformBufferObject {
        .transformation = m_camera->transformation(size.x / size.y),
        .time = time,
        .mouse = { 0, 0 }
    };

    void* data;

    ::vkMapMemory(m_graphicsObject->logicalDevice(), m_commonGlobalUniformBufferBundles[currentImage].memory, 0, sizeof(ubo), 0, &data);
    std::memcpy(data, &ubo, sizeof(ubo));
    ::vkUnmapMemory(m_graphicsObject->logicalDevice(), m_commonGlobalUniformBufferBundles[currentImage].memory);
}

void Renderer::proceedCommandBuffers(const vk::RenderPass& renderPass,
    const vk::Extent2D& extent,
    const PerspectiveCamera& camera,
    const std::vector<vk::Framebuffer>& swapChainFramebuffers,
    const std::vector<vk::CommandBuffer>& commandBuffers,
    const std::vector<BufferBundle>& commonGlobalUniformBufferBundles,
    const std::vector<BufferBundle>& lightingUniformBufferBundles,
    const std::list<VertexObject*>& vertexObjects)
{
    for (size_t i = 0; i < commandBuffers.size(); i++) {
        vk::CommandBufferBeginInfo beginInfo{};
        if (commandBuffers[i].begin(&beginInfo) != vk::Result::eSuccess) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        //#1A0033
        const vk::ClearValue clearColor = vk::ClearColorValue(std::array<float, 4> {
                                                                  0x1a / 256.,
                                                                  0x00 / 256.,
                                                                  0x33 / 256.,
                                                                  0.4f
                                                              });

        vk::RenderPassBeginInfo renderPassInfo;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = vk::Offset2D();
        renderPassInfo.renderArea.extent = extent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vk::Viewport viewport;
        viewport.setX(0);
        viewport.setWidth(extent.width);
        viewport.setY(0);
        viewport.setHeight(extent.height);
        viewport.setMinDepth(camera.near());
        viewport.setMaxDepth(camera.far());

        commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
        commandBuffers[i].setViewport(0, 1, &viewport);

        for (auto object : vertexObjects) {
            object->draw(i, commandBuffers, commonGlobalUniformBufferBundles, lightingUniformBufferBundles);
        }

        //        vk::ImageBlit blit;
        //        commandBuffers[i].blitImage(fgImage, vk::ImageLayout::eUndefined, swapChainImages[i], vk::ImageLayout::eTransferDstOptimal, { blit }, vk::Filter::eLinear);
        commandBuffers[i].endRenderPass();
        commandBuffers[i].end();

    }
}

void Renderer::resetCommandBuffers(const std::vector<vk::CommandBuffer>& commandBuffers, const vk::Queue& graphicsQueue, const vk::Queue& presentQueue)
{
    graphicsQueue.waitIdle();
    presentQueue.waitIdle();

    for(auto b : commandBuffers) {
        b.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    }
}

VertexObject* Renderer::addObject(const BadgerEngine::Geometry::Mesh& mesh, Shared<e172vp::Pipeline> pipeline)
{
    const auto r = new VertexObject(
        m_graphicsObject,
        m_graphicsObject->swapChain().imageCount(),
        &m_objectDescriptorSetLayout,
        &m_samplerDescriptorSetLayout,
        mesh,
        m_font->character('F').imageView(),
        pipeline,
        m_normalDebugPipeline);
    m_vertexObjects.push_back(r);
    return r;
}

VertexObject* Renderer::addCharacter(char c, std::shared_ptr<e172vp::Pipeline> pipeline)
{
    const static std::vector<Geometry::Vertex> v = {
        { { -0.1f, -0.1f, 0 }, { 0, 0, 0 }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
        { { 0.1f, -0.1f, 0 }, { 0, 0, 0 }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
        { { 0.1f, 0.1f, 0 }, { 0, 0, 0 }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
        { { -0.1f, 0.1f, 0 }, { 0, 0, 0 }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } }
    };
    const static std::vector<uint32_t> i = {
        0, 1, 2,
        2, 3, 0
    };

    const auto r = new VertexObject(
        m_graphicsObject,
        m_graphicsObject->swapChain().imageCount(),
        &m_objectDescriptorSetLayout,
        &m_samplerDescriptorSetLayout,
        Geometry::Mesh(Geometry::Topology::TriangleList, v, i),
        m_font->character(c).imageView(),
        pipeline,
        m_normalDebugPipeline);
    m_vertexObjects.push_back(r);
    return r;
}

VertexObject* Renderer::addObject(const BadgerEngine::Model& model)
{
    return addObject(model.mesh(), createPipeline(model.vert(), model.frag(), Geometry::Topology::TriangleList));
}

bool Renderer::removeVertexObject(VertexObject* vertexObject)
{
    const auto it = std::find(m_vertexObjects.begin(), m_vertexObjects.end(), vertexObject);
    if (it != m_vertexObjects.end()) {
        delete vertexObject;
        m_vertexObjects.erase(it);
        return true;
    }
    return true;
}

void Renderer::createSyncObjects(const vk::Device& logicDevice, vk::Semaphore* imageAvailableSemaphore, vk::Semaphore* renderFinishedSemaphore)
{
    vk::SemaphoreCreateInfo semaphoreInfo;
    if (
            logicDevice.createSemaphore(&semaphoreInfo, nullptr, imageAvailableSemaphore) != vk::Result::eSuccess ||
            logicDevice.createSemaphore(&semaphoreInfo, nullptr, renderFinishedSemaphore) != vk::Result::eSuccess
            )
        throw std::runtime_error("failed to create synchronization objects for a frame!");
}
}
