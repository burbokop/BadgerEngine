#pragma once

#include "../Utils/NoNull.h"
#include "../Window.h"

namespace BadgerEngine {

class GLFWWindow : public Window, public InputProvider {
public:
    GLFWWindow(const std::string& title, std::size_t w, std::size_t h);
    ~GLFWWindow();

    bool shouldClose() const;
    void setMousePosition(const glm::vec2& pos);
    void setCursorVisible(bool v);

    // Window interface
public:
    glm::vec2 size() const override;
    Expected<vk::SurfaceKHR> createVulkanSurface(vk::Instance i) override;
    std::vector<std::string> requiredVulkanExtensions() const override;

    // InputProvider interface
public:
    bool pressed(Key key) const override;
    glm::vec2 mousePosition() const override;

private:
    class Impl;

private:
    Unique<Impl> m_impl;
};

}
