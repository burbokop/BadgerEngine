#include "hardware.h"

#include "extensiontools.h"
#include "stringvector.h"

bool e172vp::Hardware::isDeviceSuitable(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, const std::vector<std::string>& requiredDeviceExtensions)
{
    const auto missing = StringVector::subtract(requiredDeviceExtensions, Extension::presentDeviceExtensions(physicalDevice));
    bool swapChainAdequate = false;
    const auto features = physicalDevice.getFeatures();
    if (missing.size() == 0) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    return swapChainAdequate && missing.size() == 0 && features.samplerAnisotropy;
}

e172vp::Hardware::SwapChainSupportDetails e172vp::Hardware::querySwapChainSupport(const vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR surface)
{
    SwapChainSupportDetails details;
    {
        const auto result = physicalDevice.getSurfaceCapabilitiesKHR(surface, &details.capabilities);
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("[error] Failed to get surface capabilities: " + vk::to_string(result));
        }
    }

    // GETTING FORMATS
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        const auto result = physicalDevice.getSurfaceFormatsKHR(surface, &formatCount, details.formats.data());
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("[error] Failed to get surface formats: " + vk::to_string(result));
        }
    }
    // GETTING FORMATS END

    // GETTING PRESENT MODES
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        const auto result = physicalDevice.getSurfacePresentModesKHR(surface, &presentModeCount, details.presentModes.data());
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("[error] Failed to get surface present modes: " + vk::to_string(result));
        }
    }
    // GETTING PRESENT MODES END

    return details;
}

vk::PhysicalDevice e172vp::Hardware::findSuitablePhysicalDevice(const vk::Instance& instance, vk::SurfaceKHR surface, const std::vector<std::string>& requiredDeviceExtensions)
{
    const auto devices = instance.enumeratePhysicalDevices();
    for (const auto& d : devices) {
        if (isDeviceSuitable(d, surface, requiredDeviceExtensions))
            return d;
    }
    return nullptr;
}

e172vp::Hardware::QueueFamilies e172vp::Hardware::queryQueueFamilies(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface)
{
    QueueFamilies result;
    const auto queueFamilies = physicalDevice.getQueueFamilyProperties();

    for (std::uint32_t i = 0; const auto& queueFamily : queueFamilies) {
        vk::Bool32 presentSupport = false;

        const auto r = physicalDevice.getSurfaceSupportKHR(i, surface, &presentSupport);
        if (r != vk::Result::eSuccess) {
            throw std::runtime_error("[error] Failed to get surface capabilities: " + vk::to_string(r));
        }

        if (presentSupport)
            result.m_presentFamily = i;

        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            result.m_graphicsFamily = i;

        if (result.isValid())
            break;

        i++;
    }

    return result;
}
