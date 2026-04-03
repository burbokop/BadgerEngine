#include "Renderer.h"

#include "Camera.h"
#include "Impl/BSDFVertexObject.h"
#include "Impl/Buffers/BufferUtils.h"
#include "Impl/ImageViewVertexObject.h"
#include "Impl/UploadedTexture.h"
#include "Impl/font.h"
#include "Impl/graphicsobject.h"
#include "Impl/stringvector.h"
#include "Model/Model.h"
#include "PointLight.h"
#include "Utils/NumericCast.h"
#include "Utils/Visit.h"
#include "VertexObject.h"
#include "Window.h"
#include <BSDF.frag.spv.h>
#include <BSDF.vert.spv.h>
#include <Normal.frag.spv.h>
#include <Normal.vert.spv.h>
#include <ShadowMap.vert.spv.h>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <math.h>

#ifdef BADGER_ENGINE_RENDERDOC
#include <dlfcn.h>
#include <renderdoc_app.h>
#endif

namespace BadgerEngine {

namespace {

#ifdef BADGER_ENGINE_RENDERDOC

RENDERDOC_API_1_1_2* createRenderDocApi()
{
    RENDERDOC_API_1_1_2* result = NULL;
    if (void* mod = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD)) {
        pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)dlsym(mod, "RENDERDOC_GetAPI");
        int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)&result);
        assert(ret == 1);
    }
    return result;
}

class RenderDocGuard {
public:
    RenderDocGuard(const RenderDocGuard&) = delete;
    RenderDocGuard(RenderDocGuard&&) = delete;
    RenderDocGuard& operator=(const RenderDocGuard&) = delete;
    RenderDocGuard& operator=(RenderDocGuard&&) = delete;

    RenderDocGuard(RENDERDOC_API_1_1_2* api)
        : m_api(api)
    {
        if (m_api) {
            m_api->StartFrameCapture(NULL, NULL);
        }
    }

    ~RenderDocGuard()
    {
        if (m_api) {
            m_api->EndFrameCapture(NULL, NULL);
        }
    }

private:
    RENDERDOC_API_1_1_2* m_api;
};

#endif

template<size_t N>
class Padding {
public:
    Padding()
    {
    }

private:
    char data[N] = {};
};

struct PointLightUniformBufferObject {
    glm::vec4 position;
    glm::vec4 color;
};

static_assert(offsetof(PointLightUniformBufferObject, position) == 0, "Offset must comply with std140");
static_assert(offsetof(PointLightUniformBufferObject, color) == 16, "Offset must comply with std140");

struct AmbientLightUniformBufferObject {
    glm::vec3 color;
    float intensity;
};

static_assert(offsetof(AmbientLightUniformBufferObject, color) == 0, "Offset must comply with std140");
static_assert(offsetof(AmbientLightUniformBufferObject, intensity) == 12, "Offset must comply with std140");

struct DirectionalLightUniformBufferObject {
    glm::vec3 vector;
    Padding<4> _;
    glm::vec3 color;
    float intensity;
    glm::mat4 shadowMapTransformation;
};

static_assert(offsetof(DirectionalLightUniformBufferObject, vector) == 0, "Offset must comply with std140");
static_assert(offsetof(DirectionalLightUniformBufferObject, color) == 16, "Offset must comply with std140");
static_assert(offsetof(DirectionalLightUniformBufferObject, intensity) == 28, "Offset must comply with std140");
static_assert(offsetof(DirectionalLightUniformBufferObject, shadowMapTransformation) == 32, "Offset must comply with std140");

struct LightingUniformBufferObject {
    PointLightUniformBufferObject lights[64];
    std::uint32_t lightsCount;
    Padding<12> _;
    AmbientLightUniformBufferObject ambient;
    DirectionalLightUniformBufferObject directionalLight;
};

static_assert(offsetof(LightingUniformBufferObject, lights) == 0, "Offset must comply with std140");
static_assert(offsetof(LightingUniformBufferObject, lightsCount) == 2048, "Offset must comply with std140");
static_assert(offsetof(LightingUniformBufferObject, ambient) == 2064, "Offset must comply with std140");
static_assert(offsetof(LightingUniformBufferObject, directionalLight) == 2080, "Offset must comply with std140");

struct ColorGlobalUniformBufferObject {
    glm::mat4 transformation;
    float time;
    Padding<4> _;
    glm::vec2 mouse;
    glm::vec3 cameraPosition;
    std::uint32_t mode;
};

static_assert(offsetof(ColorGlobalUniformBufferObject, transformation) == 0, "Offset must comply with std140");
static_assert(offsetof(ColorGlobalUniformBufferObject, time) == 64, "Offset must comply with std140");
static_assert(offsetof(ColorGlobalUniformBufferObject, mouse) == 72, "Offset must comply with std140");
static_assert(offsetof(ColorGlobalUniformBufferObject, cameraPosition) == 80, "Offset must comply with std140");
static_assert(offsetof(ColorGlobalUniformBufferObject, mode) == 92, "Offset must comply with std140");

struct ShadowMapGlobalUniformBufferObject {
    glm::mat4 transformation;
};

static_assert(offsetof(ShadowMapGlobalUniformBufferObject, transformation) == 0, "Offset must comply with std140");

glm::vec2 vkExtent2DToGLMVec2(vk::Extent2D ex)
{
    return { ex.width, ex.height };
}

Expected<Shared<UploadedTexture>> uploadMaterialColorChannel(
    const MaterialColorChannel& channel,
    const e172vp::GraphicsObject& graphicsObject,
    UploadedTextureCache& cache)
{
    return std::visit(
        Overloaded {
            [&graphicsObject, &cache](const SharedTexture& texture) -> Expected<Shared<UploadedTexture>> {
                return UploadedTexture::upload(
                    graphicsObject.logicalDevice(),
                    graphicsObject.physicalDevice(),
                    graphicsObject.commandPool(),
                    // Assuming graphics queue can also do copy. If not then add some check
                    graphicsObject.graphicsQueue(),
                    cache,
                    texture)
                    .transform_error(AsReason("Failed to upload a texture"));
            },
            [&graphicsObject, &cache](const RGBAColor& color) -> Expected<Shared<UploadedTexture>> {
                return UploadedTexture::create(
                    graphicsObject.logicalDevice(),
                    graphicsObject.physicalDevice(),
                    graphicsObject.commandPool(),
                    // Assuming graphics queue can also do copy. If not then add some check
                    graphicsObject.graphicsQueue(),
                    cache,
                    PixFormat::RGBA32,
                    { 1, 1 },
                    color)
                    .transform_error(AsReason("Failed to create a texture from color"));
            },
        },
        channel);
}

void resetCommandBuffers(const std::vector<vk::CommandBuffer>& commandBuffers, const vk::Queue& graphicsQueue, const vk::Queue& presentQueue)
{
    graphicsQueue.waitIdle();
    presentQueue.waitIdle();

    for (auto b : commandBuffers) {
        b.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    }
}

void createSyncObjects(const vk::Device& logicDevice, vk::Semaphore* imageAvailableSemaphore, vk::Semaphore* renderFinishedSemaphore)
{
    vk::SemaphoreCreateInfo semaphoreInfo;
    if (
        logicDevice.createSemaphore(&semaphoreInfo, nullptr, imageAvailableSemaphore) != vk::Result::eSuccess || logicDevice.createSemaphore(&semaphoreInfo, nullptr, renderFinishedSemaphore) != vk::Result::eSuccess)
        throw std::runtime_error("failed to create synchronization objects for a frame!");
}

}

struct RendererImpl {
    Shared<e172vp::GraphicsObject> graphicsObject;

    vk::Semaphore imageAvailableSemaphore;
    vk::Semaphore renderFinishedSemaphore;

    vk::Buffer vertexBuffer;
    vk::DeviceMemory vertexBufferMemory;
    vk::Buffer indexBuffer;
    vk::DeviceMemory indexBufferMemory;

    e172vp::DescriptorSetLayout colorGlobalDescriptorSetLayout;
    e172vp::DescriptorSetLayout lightingDescriptorSetLayout;
    e172vp::DescriptorSetLayout shadowMapGlobalDescriptorSetLayout;
    e172vp::DescriptorSetLayout objectDescriptorSetLayout;
    e172vp::DescriptorSetLayout baseColorSamplerDescriptorSetLayout;
    e172vp::DescriptorSetLayout ambientOcclusionDescriptorSetLayout;
    e172vp::DescriptorSetLayout normalMapDescriptorSetLayout;
    e172vp::DescriptorSetLayout shadowMapSamplerDescriptorSetLayout;

    std::vector<BufferBundle> colorGlobalUniformBufferBundles;
    std::vector<BufferBundle> lightingUniformBufferBundles;
    std::vector<BufferBundle> shadowMapGlobalUniformBufferBundles;

    e172vp::Font* font = nullptr;
    std::list<VertexObject*> vertexObjects;
    std::shared_ptr<e172vp::Pipeline> normalDebugPipeline;
    std::shared_ptr<e172vp::Pipeline> shadowMapPipeline;
    std::vector<Shared<PointLight>> pointLights;
    DirectionBasedOrthographicCamera directionalLightCamera;

    Shared<Window> window;
    Shared<UploadedTextureCache> textureCache;
    std::shared_ptr<UploadedTexture> defaultTexture;

#ifdef BADGER_ENGINE_RENDERDOC
    RENDERDOC_API_1_1_2* renderDocApi;
#endif
};

Renderer::Renderer(Shared<Window> window, Shared<Camera> camera, std::span<const std::uint8_t> fontBytes)
    : m_impl(std::make_shared<RendererImpl>(RendererImpl {
          .graphicsObject = std::make_shared<e172vp::GraphicsObject>(e172vp::GraphicsObjectCreateInfo {
              .applicationName = "badger_engine_app",
              .applicationVersion = 1,
              .requiredExtensions = window->requiredVulkanExtensions(),
              .requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_EXT_memory_budget" },
              .surfaceCreator = [window](vk::Instance i, vk::SurfaceKHR* s) {
                  *s = window->createVulkanSurface(i).transform_error(AsCritical()).value();
              },
              .descriptorPoolSize = 1024 * 64,
#ifndef NDEBUG
              .debugEnabled = true,
#else
              .debugEnabled = false,
#endif
          }),
          .imageAvailableSemaphore = {},
          .renderFinishedSemaphore = {},
          .vertexBuffer = {},
          .vertexBufferMemory = {},
          .indexBuffer = {},
          .indexBufferMemory = {},

          .colorGlobalDescriptorSetLayout = {},
          .lightingDescriptorSetLayout = {},
          .shadowMapGlobalDescriptorSetLayout = {},
          .objectDescriptorSetLayout = {},
          .baseColorSamplerDescriptorSetLayout = {},
          .ambientOcclusionDescriptorSetLayout = {},
          .normalMapDescriptorSetLayout = {},
          .shadowMapSamplerDescriptorSetLayout = {},

          .colorGlobalUniformBufferBundles = {},
          .lightingUniformBufferBundles = {},
          .shadowMapGlobalUniformBufferBundles = {},
          .font = nullptr,
          .vertexObjects = {},
          .normalDebugPipeline = {},
          .shadowMapPipeline = {},
          .pointLights = {},
          .directionalLightCamera = {},

          .window = std::move(window),
          .textureCache = std::make_shared<UploadedTextureCache>(),
          .defaultTexture = {},

#ifdef BADGER_ENGINE_RENDERDOC
          .renderDocApi = createRenderDocApi()
#endif
      }))
    , m_camera(std::move(camera))
    , m_directionalLight(m_impl)
{

    if (m_impl->graphicsObject->debugEnabled())
        std::cout << "Used validation layers: " << e172vp::StringVector::toString(m_impl->graphicsObject->enabledValidationLayers()) << "\n";

    if (!m_impl->graphicsObject->isValid())
        std::cout << "GRAPHICS OBJECT IS NOT CREATED BECAUSE OF FOLOWING ERRORS:\n\n";

    const auto errors = m_impl->graphicsObject->pullErrors();
    if (errors.size() > 0) {
        std::cerr << e172vp::StringVector::toString(errors) << "\n";
    }

    m_impl->colorGlobalDescriptorSetLayout = e172vp::DescriptorSetLayout::createUniformDSL(m_impl->graphicsObject->logicalDevice(), 0);
    m_impl->lightingDescriptorSetLayout = e172vp::DescriptorSetLayout::createUniformDSL(m_impl->graphicsObject->logicalDevice(), 0);
    m_impl->shadowMapGlobalDescriptorSetLayout = e172vp::DescriptorSetLayout::createUniformDSL(m_impl->graphicsObject->logicalDevice(), 0);
    m_impl->objectDescriptorSetLayout = e172vp::DescriptorSetLayout::createUniformDSL(m_impl->graphicsObject->logicalDevice(), 0);
    m_impl->baseColorSamplerDescriptorSetLayout = e172vp::DescriptorSetLayout::createSamplerDSL(m_impl->graphicsObject->logicalDevice(), 0);
    m_impl->ambientOcclusionDescriptorSetLayout = e172vp::DescriptorSetLayout::createSamplerDSL(m_impl->graphicsObject->logicalDevice(), 0);
    m_impl->normalMapDescriptorSetLayout = e172vp::DescriptorSetLayout::createSamplerDSL(m_impl->graphicsObject->logicalDevice(), 0);
    m_impl->shadowMapSamplerDescriptorSetLayout = e172vp::DescriptorSetLayout::createSamplerDSL(m_impl->graphicsObject->logicalDevice(), 0);

    m_impl->colorGlobalUniformBufferBundles = BufferUtils::createUniformBufferBundle<ColorGlobalUniformBufferObject>(
        *m_impl->graphicsObject,
        m_impl->graphicsObject->swapChain().imageCount(),
        m_impl->graphicsObject->descriptorPool(),
        m_impl->colorGlobalDescriptorSetLayout);

    m_impl->lightingUniformBufferBundles = BufferUtils::createUniformBufferBundle<LightingUniformBufferObject>(
        *m_impl->graphicsObject,
        m_impl->graphicsObject->swapChain().imageCount(),
        m_impl->graphicsObject->descriptorPool(),
        m_impl->lightingDescriptorSetLayout);

    m_impl->shadowMapGlobalUniformBufferBundles = BufferUtils::createUniformBufferBundle<ShadowMapGlobalUniformBufferObject>(
        *m_impl->graphicsObject,
        m_impl->graphicsObject->swapChain().imageCount(),
        m_impl->graphicsObject->descriptorPool(),
        m_impl->shadowMapGlobalDescriptorSetLayout);

    createSyncObjects(m_impl->graphicsObject->logicalDevice(), &m_impl->imageAvailableSemaphore, &m_impl->renderFinishedSemaphore);

    if (!fontBytes.empty()) {
        m_impl->font = new e172vp::Font(
            m_impl->graphicsObject->logicalDevice(),
            m_impl->graphicsObject->physicalDevice(),
            m_impl->graphicsObject->commandPool(),
            m_impl->graphicsObject->graphicsQueue(),
            fontBytes,
            128);
    }

    m_impl->defaultTexture = UploadedTexture::create(
        m_impl->graphicsObject->logicalDevice(),
        m_impl->graphicsObject->physicalDevice(),
        m_impl->graphicsObject->commandPool(),
        // Assuming graphics queue can also do copy. If not then add some check
        m_impl->graphicsObject->graphicsQueue(),
        *m_impl->textureCache,
        PixFormat::RGBA32,
        { 1, 1 },
        glm::vec4(1, 0, 1, 1))
                                 .transform_error(AsCritical())
                                 .value()
                                 .nullable();

    m_impl->normalDebugPipeline = createColorPipeline(
        Normal_vert,
        Normal_frag,
        Geometry::Topology::LineList,
        PolygonMode::Fill,
        true);

    m_impl->shadowMapPipeline = createShadowMapPipeline(
        ShadowMap_vert,
        Geometry::Topology::TriangleList,
        PolygonMode::Fill,
        false);
}

std::shared_ptr<e172vp::Pipeline> Renderer::createColorPipeline(
    std::span<const uint8_t> vertShaderCode,
    std::span<const uint8_t> fragShaderCode,
    Geometry::Topology topology,
    BadgerEngine::PolygonMode polygonMode,
    bool backfaceCulling)
{
    return std::make_shared<e172vp::Pipeline>(
        m_impl->graphicsObject->logicalDevice(),
        m_impl->graphicsObject->swapChainSettings().extent,
        m_impl->graphicsObject->colorRenderPass()->handle(),
        std::vector {
            m_impl->colorGlobalDescriptorSetLayout.descriptorSetLayoutHandle(),
            m_impl->lightingDescriptorSetLayout.descriptorSetLayoutHandle(),
            m_impl->objectDescriptorSetLayout.descriptorSetLayoutHandle(),
            m_impl->baseColorSamplerDescriptorSetLayout.descriptorSetLayoutHandle(),
            m_impl->ambientOcclusionDescriptorSetLayout.descriptorSetLayoutHandle(),
            m_impl->normalMapDescriptorSetLayout.descriptorSetLayoutHandle(),
            m_impl->shadowMapSamplerDescriptorSetLayout.descriptorSetLayoutHandle(),
        },
        vertShaderCode,
        fragShaderCode,
        topology,
        polygonMode,
        backfaceCulling);
}

std::shared_ptr<e172vp::Pipeline> Renderer::createShadowMapAsColorPipeline(
    std::span<const uint8_t> vertShaderCode,
    std::span<const uint8_t> fragShaderCode,
    Geometry::Topology topology,
    PolygonMode polygonMode,
    bool backfaceCulling)
{
    return std::make_shared<e172vp::Pipeline>(
        m_impl->graphicsObject->logicalDevice(),
        m_impl->graphicsObject->swapChainSettings().extent,
        m_impl->graphicsObject->colorRenderPass()->handle(),
        std::vector {
            m_impl->colorGlobalDescriptorSetLayout.descriptorSetLayoutHandle(),
            m_impl->lightingDescriptorSetLayout.descriptorSetLayoutHandle(),
            m_impl->objectDescriptorSetLayout.descriptorSetLayoutHandle(),
            m_impl->shadowMapSamplerDescriptorSetLayout.descriptorSetLayoutHandle(),
        },
        vertShaderCode,
        fragShaderCode,
        topology,
        polygonMode,
        backfaceCulling);
}

std::shared_ptr<e172vp::Pipeline> Renderer::createShadowMapPipeline(
    std::span<const uint8_t> vertShaderCode,
    Geometry::Topology topology,
    PolygonMode polygonMode,
    bool backfaceCulling)
{
    return std::make_shared<e172vp::Pipeline>(
        m_impl->graphicsObject->logicalDevice(),
        m_impl->graphicsObject->swapChainSettings().shadowMapExtent,
        m_impl->graphicsObject->shadowMapRenderPass()->handle(),
        std::vector {
            m_impl->shadowMapGlobalDescriptorSetLayout.descriptorSetLayoutHandle(),
            m_impl->objectDescriptorSetLayout.descriptorSetLayoutHandle(),
        },
        vertShaderCode,
        topology,
        polygonMode,
        backfaceCulling);
}

Expected<void> Renderer::proceedRenderPass(
    std::size_t imageIndex,
    const vk::RenderPass& renderPass,
    const vk::Extent2D& extent,
    const Camera& camera,
    std::span<const vk::ClearValue> clearValues,
    const std::vector<vk::Framebuffer>& swapChainFramebuffers,
    const std::vector<vk::CommandBuffer>& commandBuffers,
    const std::vector<BufferBundle>& commonGlobalUniformBufferBundles,
    const std::vector<BufferBundle>& lightingUniformBufferBundles,
    const std::list<VertexObject*>& vertexObjects,
    VertexObject::RenderTarget renderTarget) noexcept
{
    const vk::RenderPassBeginInfo renderPassInfo = {
        .renderPass = renderPass,
        .framebuffer = swapChainFramebuffers[imageIndex],
        .renderArea = {
            .offset = vk::Offset2D(),
            .extent = extent,
        },
        .clearValueCount = numericCast<std::uint32_t>(clearValues.size()).value(),
        .pClearValues = clearValues.data(),
    };

    const vk::Viewport viewport = {
        .x = 0,
        .y = 0,
        .width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = camera.near(),
        .maxDepth = camera.far(),
    };

    commandBuffers[imageIndex].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
    commandBuffers[imageIndex].setViewport(0, 1, &viewport);

    for (auto object : vertexObjects) {
        const auto result = object->draw(imageIndex, commandBuffers, commonGlobalUniformBufferBundles, lightingUniformBufferBundles, renderTarget);
        if (!result) {
            return unexpected("Failed to draw vertex object", result.error());
        }
    }

    commandBuffers[imageIndex].endRenderPass();

    return {};
}

Expected<void> Renderer::fillCommandBuffers()
{
    const auto commandBuffers = m_impl->graphicsObject->commandPool().commandBufferVector();

    for (size_t i = 0; i < commandBuffers.size(); i++) {
        vk::CommandBufferBeginInfo beginInfo;
        {
            const auto result = commandBuffers[i].begin(&beginInfo);
            if (result != vk::Result::eSuccess) {
                return unexpected("Failed to begin recording command buffer: " + vk::to_string(result));
            }
        }

        {
            const auto result = proceedRenderPass(
                i,
                m_impl->graphicsObject->shadowMapRenderPass()->handle(),
                m_impl->graphicsObject->swapChainSettings().shadowMapExtent,
                m_impl->directionalLightCamera,
                std::array<vk::ClearValue, 1> { vk::ClearDepthStencilValue(1.0f, 0.f) },
                m_impl->graphicsObject->swapChain().shadowMapFrameBufferVector(),
                commandBuffers,
                m_impl->shadowMapGlobalUniformBufferBundles,
                {},
                m_impl->vertexObjects,
                VertexObject::RenderTarget::ShadowMap);

            if (!result) {
                return unexpected("Failed to process render pass", result.error());
            }
        }

        {
            const auto result = proceedRenderPass(
                i,
                m_impl->graphicsObject->colorRenderPass()->handle(),
                m_impl->graphicsObject->swapChainSettings().extent,
                *m_camera,
                std::array<vk::ClearValue, 2> {
                    // #1A0033
                    vk::ClearColorValue(std::array<float, 4> {
                        0x1a / 256.,
                        0x00 / 256.,
                        0x33 / 256.,
                        0.4f,
                    }),
                    vk::ClearDepthStencilValue(1.0f, 0.f),
                },
                m_impl->graphicsObject->swapChain().frameBufferVector(),
                commandBuffers,
                m_impl->colorGlobalUniformBufferBundles,
                m_impl->lightingUniformBufferBundles,
                m_impl->vertexObjects,
                VertexObject::RenderTarget::Color);

            if (!result) {
                return unexpected("Failed to process render pass", result.error());
            }
        }

        commandBuffers[i].end();
    }

    return {};
}

Expected<void> Renderer::applyPresentation() noexcept
{
#ifdef BADGER_ENGINE_RENDERDOC
    RenderDocGuard _(m_impl->renderDocApi);
#endif

    resetCommandBuffers(
        m_impl->graphicsObject->commandPool().commandBufferVector(),
        m_impl->graphicsObject->graphicsQueue(),
        m_impl->graphicsObject->presentQueue());

    {
        const auto result = fillCommandBuffers();
        if (!result) {
            return unexpected("Failed to fill command buffers", result.error());
        }
    }

    std::uint32_t imageIndex = 0;

    {
        const auto result = m_impl
                                ->graphicsObject
                                ->logicalDevice()
                                .acquireNextImageKHR(
                                    m_impl->graphicsObject->swapChain().swapChainHandle(),
                                    UINT64_MAX,
                                    m_impl->imageAvailableSemaphore,
                                    {},
                                    &imageIndex);

        if (result != vk::Result::eSuccess) {
            return unexpected("Failed to acquire next image: " + vk::to_string(result));
        }
    }

    auto currentImageCommandBuffer = m_impl->graphicsObject->commandPool().commandBuffer(imageIndex);

    vk::Semaphore waitSemaphores[] = { m_impl->imageAvailableSemaphore };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

    updateUniformBuffer(imageIndex);

    for (auto& o : m_impl->vertexObjects) {
        const auto result = o->updateUniformBuffer(imageIndex);
        if (!result) {
        }
    }

    vk::SubmitInfo submitInfo;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.setCommandBuffers(currentImageCommandBuffer);
    submitInfo.setSignalSemaphores(m_impl->renderFinishedSemaphore);

    {
        const auto result = m_impl->graphicsObject->graphicsQueue().submit(1, &submitInfo, {});
        if (result != vk::Result::eSuccess) {
            return unexpected("Failed to submit draw command buffer: " + vk::to_string(result));
        }
    }

    vk::SwapchainKHR swapChains[] = { m_impl->graphicsObject->swapChain().swapChainHandle() };

    vk::PresentInfoKHR presentInfo;
    presentInfo.setWaitSemaphores(m_impl->renderFinishedSemaphore);
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    {
        const auto result = m_impl->graphicsObject->presentQueue().presentKHR(&presentInfo);
        if (result != vk::Result::eSuccess) {
            return unexpected("Present failed: " + vk::to_string(result));
        }
    }

    return {};
}

DirectionalLight& Renderer::directionalLight()
{
    return m_directionalLight;
}

const DirectionalLight& Renderer::directionalLight() const
{
    return m_directionalLight;
}

void DirectionalLight::setDirection(glm::vec3 v)
{
    m_direction = v;
    m_impl->directionalLightCamera.setOrbit(m_shadowFocus, m_direction);
}

void DirectionalLight::setShadowFocus(glm::vec3 v)
{
    m_shadowFocus = v;
    m_impl->directionalLightCamera.setOrbit(m_shadowFocus, m_direction);
}

void DirectionalLight::setShadowNear(float v)
{
    m_impl->directionalLightCamera.setNear(v);
    m_impl->directionalLightCamera.setOrbit(m_shadowFocus, m_direction);
}

void DirectionalLight::setShadowFar(float v)
{
    m_impl->directionalLightCamera.setFar(v);
    m_impl->directionalLightCamera.setOrbit(m_shadowFocus, m_direction);
}

void DirectionalLight::setShadowCameraScale(float v)
{
    m_impl->directionalLightCamera.setScale(v);
    m_impl->directionalLightCamera.setOrbit(m_shadowFocus, m_direction);
}

void DirectionalLight::setShadowBias(float v)
{
    m_shadowBias = v;
}

glm::vec3 DirectionalLight::shadowCameraPosition() const
{
    return m_impl->directionalLightCamera.position();
}

void Renderer::updateUniformBuffer(uint32_t currentImage)
{
    const auto directionalLightCameraTransformation
        = m_impl
              ->directionalLightCamera
              .transformation(vkExtent2DToGLMVec2(m_impl->graphicsObject->swapChainSettings().shadowMapExtent));

    {
        static const auto begin = std::chrono::high_resolution_clock::now();
        const float time = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin).count()) / 1000.f;

        const auto ubo = ColorGlobalUniformBufferObject {
            .transformation = m_camera->transformation(vkExtent2DToGLMVec2(m_impl->graphicsObject->swapChainSettings().extent)),
            .time = time,
            ._ = {},
            .mouse = { 0, 0 },
            .cameraPosition = m_camera->position(),
            .mode = m_mode,
        };

        void* data = nullptr;
        const auto result = m_impl->graphicsObject->logicalDevice().mapMemory(m_impl->colorGlobalUniformBufferBundles[currentImage].memory, 0, sizeof(ubo), vk::MemoryMapFlags(), &data);
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to map memory: " + vk::to_string(result));
        }
        std::memcpy(data, &ubo, sizeof(ubo));
        m_impl->graphicsObject->logicalDevice().unmapMemory(m_impl->colorGlobalUniformBufferBundles[currentImage].memory);
    }

    {
        LightingUniformBufferObject ubo;
        constexpr std::size_t capacity = sizeof(ubo.lights) / sizeof(ubo.lights[0]);
        assert(m_impl->pointLights.size() <= capacity);
        ubo.lightsCount = numericCast<std::uint32_t>(std::min(capacity, m_impl->pointLights.size())).value();

        for (std::size_t i = 0; i < ubo.lightsCount; ++i) {
            ubo.lights[i] = PointLightUniformBufferObject {
                .position = glm::vec4(m_impl->pointLights[i]->position, 0.f),
                .color = glm::vec4(m_impl->pointLights[i]->color, m_impl->pointLights[i]->intensity),
            };
        }

        ubo.ambient = AmbientLightUniformBufferObject {
            .color = glm::vec3(1),
            .intensity = 0.05f,
        };

        const auto textureSpaceTransformation = glm::translate(glm::mat4(1.), { 0.5, 0.5, -m_directionalLight.shadowBias() }) * glm::scale(glm::mat4(1.), { 0.5, 0.5, 1 });

        ubo.directionalLight = DirectionalLightUniformBufferObject {
            .vector = m_directionalLight.direction(),
            ._ = {},
            .color = m_directionalLight.color(),
            .intensity = m_directionalLight.intensity(),
            .shadowMapTransformation = textureSpaceTransformation * directionalLightCameraTransformation
        };

        void* data = nullptr;
        const auto result = m_impl->graphicsObject->logicalDevice().mapMemory(m_impl->lightingUniformBufferBundles[currentImage].memory, 0, sizeof(ubo), vk::MemoryMapFlags(), &data);
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to map memory: " + vk::to_string(result));
        }
        std::memcpy(data, &ubo, sizeof(ubo));
        m_impl->graphicsObject->logicalDevice().unmapMemory(m_impl->lightingUniformBufferBundles[currentImage].memory);
    }

    {
        const auto ubo = ShadowMapGlobalUniformBufferObject {
            .transformation = directionalLightCameraTransformation,
        };

        void* data = nullptr;
        const auto result = m_impl->graphicsObject->logicalDevice().mapMemory(m_impl->shadowMapGlobalUniformBufferBundles[currentImage].memory, 0, sizeof(ubo), vk::MemoryMapFlags(), &data);
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to map memory: " + vk::to_string(result));
        }
        std::memcpy(data, &ubo, sizeof(ubo));
        m_impl->graphicsObject->logicalDevice().unmapMemory(m_impl->shadowMapGlobalUniformBufferBundles[currentImage].memory);
    }
}

VertexObject& Renderer::addCharacter(char c, std::shared_ptr<e172vp::Pipeline> pipeline)
{
    const static std::vector<Geometry::Vertex> v = {
        { .position = { -0.1f, -0.1f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 0.0f, 0.0f }, .uv = { 0.0f, 0.0f } },
        { .position = { 0.1f, -0.1f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 0.0f, 1.0f, 0.0f }, .uv = { 1.0f, 0.0f } },
        { .position = { 0.1f, 0.1f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 0.0f, 0.0f, 1.0f }, .uv = { 1.0f, 1.0f } },
        { .position = { -0.1f, 0.1f, 0 }, .normal = { 0, 0, 1 }, .tangent = { 1, 0, 0 }, .bitangent = { 0, 1, 0 }, .color = { 1.0f, 1.0f, 1.0f }, .uv = { 0.0f, 1.0f } },
    };

    const static std::vector<uint32_t> i = {
        0, 1, 2,
        2, 3, 0
    };

    const auto r = new ImageViewVertexObject(
        m_impl->graphicsObject,
        m_impl->graphicsObject->swapChain().imageCount(),
        m_impl->objectDescriptorSetLayout,
        m_impl->baseColorSamplerDescriptorSetLayout,
        Geometry::Mesh::create(Geometry::Topology::TriangleList, v, i),
        m_impl->font->character(c).imageView(),
        pipeline,
        m_impl->normalDebugPipeline,
        DisplayNormals::NoNormals);
    m_impl->vertexObjects.push_back(r);
    return *r;
}

Renderer::~Renderer() = default;

VertexObject& Renderer::addObject(const BadgerEngine::Model& model, RenderingOptions options)
{
    return std::visit(
        Overloaded {
            [this, &model, &options](const BSDFMaterial& material) -> VertexObject& {
                const auto baseColor = uploadMaterialColorChannel(material.baseColor, *m_impl->graphicsObject, *m_impl->textureCache).transform_error(AsCritical()).value();
                const auto ambientOclusion = uploadMaterialColorChannel(material.ambientOclusion, *m_impl->graphicsObject, *m_impl->textureCache).transform_error(AsCritical()).value();
                const auto normalMap = uploadMaterialColorChannel(material.normalMap, *m_impl->graphicsObject, *m_impl->textureCache).transform_error(AsCritical()).value();

                const auto result = new BSDFVertexObject(
                    m_impl->graphicsObject,
                    m_impl->graphicsObject->swapChain().imageCount(),
                    m_impl->objectDescriptorSetLayout,
                    m_impl->baseColorSamplerDescriptorSetLayout,
                    m_impl->ambientOcclusionDescriptorSetLayout,
                    m_impl->normalMapDescriptorSetLayout,
                    m_impl->shadowMapSamplerDescriptorSetLayout,
                    model.mesh(),
                    std::move(baseColor),
                    std::move(ambientOclusion),
                    std::move(normalMap),
                    createColorPipeline(BSDF_vert, BSDF_frag, model.mesh()->topology(), model.polygonMode(), options.backfaceCulling),
                    m_impl->normalDebugPipeline,
                    m_impl->shadowMapPipeline,
                    options.displayNormals,
                    material.castShadow);
                m_impl->vertexObjects.push_back(result);
                return *result;
            },
            [this, &model, &options](const RecursiveMaterial& material) -> VertexObject& {
                const auto& frames = m_impl->graphicsObject->swapChain().frames();
                std::size_t i = 0;
                assert(i < frames.size());
                const auto result = new ImageViewVertexObject(
                    m_impl->graphicsObject,
                    m_impl->graphicsObject->swapChain().imageCount(),
                    m_impl->objectDescriptorSetLayout,
                    m_impl->baseColorSamplerDescriptorSetLayout,
                    model.mesh(),
                    frames[i].color.imageView,
                    createColorPipeline(material.vert, material.frag, model.mesh()->topology(), model.polygonMode(), options.backfaceCulling),
                    m_impl->normalDebugPipeline,
                    options.displayNormals);
                m_impl->vertexObjects.push_back(result);
                return *result;
            },
            [this, &model, &options](const ShadowMapMaterial& material) -> VertexObject& {
                const auto result = new ImageViewVertexObject(
                    m_impl->graphicsObject,
                    m_impl->objectDescriptorSetLayout,
                    m_impl->shadowMapSamplerDescriptorSetLayout,
                    model.mesh(),
                    m_impl->graphicsObject->swapChain().shadowMapImageViewVector(),
                    vk::ImageLayout::eDepthStencilReadOnlyOptimal,
                    createShadowMapAsColorPipeline(material.vert, material.frag, model.mesh()->topology(), model.polygonMode(), options.backfaceCulling),
                    m_impl->normalDebugPipeline,
                    options.displayNormals);
                m_impl->vertexObjects.push_back(result);
                return *result;
            },
            [this, &model, &options](const CustomMaterial& material) -> VertexObject& {
                if (material.textures.size() > 0) {

                    const auto texture = UploadedTexture::upload(
                        m_impl->graphicsObject->logicalDevice(),
                        m_impl->graphicsObject->physicalDevice(),
                        m_impl->graphicsObject->commandPool(),
                        // Assuming graphics queue can also do copy. If not then add some check
                        m_impl->graphicsObject->graphicsQueue(),
                        *m_impl->textureCache,
                        material.textures.front())
                                             .transform_error(AsCritical())
                                             .value();

                    const auto result = new ImageViewVertexObject(
                        m_impl->graphicsObject,
                        m_impl->graphicsObject->swapChain().imageCount(),
                        m_impl->objectDescriptorSetLayout,
                        m_impl->baseColorSamplerDescriptorSetLayout,
                        model.mesh(),
                        texture,
                        createColorPipeline(material.vert, material.frag, model.mesh()->topology(), model.polygonMode(), options.backfaceCulling),
                        m_impl->normalDebugPipeline,
                        options.displayNormals);
                    m_impl->vertexObjects.push_back(result);
                    return *result;

                } else if (m_impl->font) {
                    const auto result = new ImageViewVertexObject(
                        m_impl->graphicsObject,
                        m_impl->graphicsObject->swapChain().imageCount(),
                        m_impl->objectDescriptorSetLayout,
                        m_impl->baseColorSamplerDescriptorSetLayout,
                        model.mesh(),
                        m_impl->font->character('N').imageView(),
                        createColorPipeline(material.vert, material.frag, model.mesh()->topology(), model.polygonMode(), options.backfaceCulling),
                        m_impl->normalDebugPipeline,
                        options.displayNormals);
                    m_impl->vertexObjects.push_back(result);
                    return *result;
                } else {
                    const auto result = new ImageViewVertexObject(
                        m_impl->graphicsObject,
                        m_impl->graphicsObject->swapChain().imageCount(),
                        m_impl->objectDescriptorSetLayout,
                        m_impl->baseColorSamplerDescriptorSetLayout,
                        model.mesh(),
                        m_impl->defaultTexture,
                        createColorPipeline(material.vert, material.frag, model.mesh()->topology(), model.polygonMode(), options.backfaceCulling),
                        m_impl->normalDebugPipeline,
                        options.displayNormals);
                    m_impl->vertexObjects.push_back(result);
                    return *result;
                }
            },
        },
        *model.material());
}

Shared<PointLight> Renderer::addPointLight(glm::vec3 position, glm::vec3 color, float intensity)
{
    m_impl->pointLights.push_back(std::make_shared<PointLight>(position, color, intensity));
    return m_impl->pointLights.back();
}

bool Renderer::removeVertexObject(VertexObject* vertexObject)
{
    const auto it = std::find(m_impl->vertexObjects.begin(), m_impl->vertexObjects.end(), vertexObject);
    if (it != m_impl->vertexObjects.end()) {
        delete vertexObject;
        m_impl->vertexObjects.erase(it);
        return true;
    }
    return true;
}

}
