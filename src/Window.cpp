#include "Window.h"

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS

#include <GLFW/glfw3.h>
#include <atomic>

namespace BadgerEngine {

class GLFWWindow::Impl {
public:
    Impl(const std::string& title, std::size_t w, std::size_t h)
        : m_window(::glfwCreateWindow(w, h, init(title.c_str()), nullptr, nullptr))
    {
    }

    glm::vec2 size() const
    {
        int w, h;
        ::glfwGetWindowSize(m_window.nullable(), &w, &h);
        return {
            static_cast<float>(w),
            static_cast<float>(h)
        };
    }

    glm::vec2 mousePosition() const
    {
        double x, y;
        ::glfwGetCursorPos(m_window.nullable(), &x, &y);
        return {
            static_cast<float>(x),
            static_cast<float>(y)
        };
    }

    bool isPressed(Key key) const
    {
        const auto state = ::glfwGetKey(m_window.nullable(), static_cast<int>(key));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool shouldClose() const
    {
        ::glfwPollEvents();
        return ::glfwWindowShouldClose(m_window.nullable());
    }

    void setCursorVisible(bool v)
    {
        ::glfwSetInputMode(m_window.nullable(), GLFW_CURSOR, v ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    void setMousePosition(const glm::vec2& pos)
    {
        ::glfwSetCursorPos(m_window.nullable(), pos.x, pos.y);
    }

    Expected<vk::SurfaceKHR> createVulkanSurface(vk::Instance i)
    {
        VkSurfaceKHR surface;
        if (::glfwCreateWindowSurface(i, m_window.nullable(), NULL, &surface) != VK_SUCCESS) {
            return unexpected("surface creating error");
        }
        return surface;
    }

    std::vector<std::string> requiredVulkanExtensions() const
    {
        std::uint32_t count = 0;
        const char** extensions = glfwGetRequiredInstanceExtensions(&count);
        assert(extensions);
        std::vector<std::string> result(count);
        for (std::uint32_t i = 0; i < count; ++i) {
            result[i] = extensions[i];
        }
        return result;
    }

    ~Impl()
    {
    }

private:
    template<typename T>
    static T init(T&& v)
    {
        static std::mutex mutex;
        std::lock_guard g(mutex);
        static bool inited = false;
        if (!inited) {
            ::glfwInit();
            ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            ::glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            inited = true;
        }
        return v;
    }

private:
    RawPtr<GLFWwindow> m_window;
};

GLFWWindow::GLFWWindow(const std::string& title, std::size_t w, std::size_t h)
    : m_impl(std::make_unique<Impl>(title, w, h))
{
}

GLFWWindow::~GLFWWindow() = default;

bool GLFWWindow::isPressed(Key key) const
{
    return m_impl->isPressed(key);
}

bool GLFWWindow::shouldClose() const
{
    return m_impl->shouldClose();
}

glm::vec2 GLFWWindow::size() const
{
    return m_impl->size();
}

glm::vec2 GLFWWindow::mousePosition() const
{
    return m_impl->mousePosition();
}

void GLFWWindow::setMousePosition(const glm::vec2& pos)
{
    m_impl->setMousePosition(pos);
}

void GLFWWindow::setCursorVisible(bool v)
{
    m_impl->setCursorVisible(v);
}

Expected<vk::SurfaceKHR> GLFWWindow::createVulkanSurface(vk::Instance i)
{
    return m_impl->createVulkanSurface(i);
}

std::vector<std::string> GLFWWindow::requiredVulkanExtensions() const
{
    return m_impl->requiredVulkanExtensions();
}

}
