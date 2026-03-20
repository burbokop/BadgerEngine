#pragma once

#include "../Utils/NoNull.h"
#include "../Window.h"

namespace BadgerEngine {

class SDL2Window : public Window {
public:
    SDL2Window(const std::string& title, std::size_t w, std::size_t h);
    ~SDL2Window();

    bool isPressed(Key key) const;
    bool shouldClose() const;
    glm::vec2 mousePosition() const;
    void setMousePosition(const glm::vec2& pos);
    [[nodiscard]] Expected<void> setCursorVisible(bool v) noexcept;

    // Window interface
public:
    glm::vec2 size() const override;
    Expected<vk::SurfaceKHR> createVulkanSurface(vk::Instance i) override;
    std::vector<std::string> requiredVulkanExtensions() const override;

private:
    class Impl;

private:
    Unique<Impl> m_impl;
};

}
