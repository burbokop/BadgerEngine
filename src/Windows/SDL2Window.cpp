#include "SDL2Window.h"

#define GLM_FORCE_RADIANS
#include "../Utils/NumericCast.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <atomic>
#include <iostream>
#include <map>

namespace BadgerEngine {

namespace {

std::optional<Window::Key> windowKeyFromSDLScancode(SDL_Scancode scancode)
{
    switch (scancode) {
    case SDL_SCANCODE_UNKNOWN:
        return std::nullopt;
    case SDL_SCANCODE_A:
        return Window::Key::A;
    case SDL_SCANCODE_B:
        return Window::Key::B;
    case SDL_SCANCODE_C:
        return Window::Key::C;
    case SDL_SCANCODE_D:
        return Window::Key::D;
    case SDL_SCANCODE_E:
        return Window::Key::E;
    case SDL_SCANCODE_F:
        return Window::Key::F;
    case SDL_SCANCODE_G:
        return Window::Key::G;
    case SDL_SCANCODE_H:
        return Window::Key::H;
    case SDL_SCANCODE_I:
        return Window::Key::I;
    case SDL_SCANCODE_J:
        return Window::Key::J;
    case SDL_SCANCODE_K:
        return Window::Key::K;
    case SDL_SCANCODE_L:
        return Window::Key::L;
    case SDL_SCANCODE_M:
        return Window::Key::M;
    case SDL_SCANCODE_N:
        return Window::Key::N;
    case SDL_SCANCODE_O:
        return Window::Key::O;
    case SDL_SCANCODE_P:
        return Window::Key::P;
    case SDL_SCANCODE_Q:
        return Window::Key::Q;
    case SDL_SCANCODE_R:
        return Window::Key::R;
    case SDL_SCANCODE_S:
        return Window::Key::S;
    case SDL_SCANCODE_T:
        return Window::Key::T;
    case SDL_SCANCODE_U:
        return Window::Key::U;
    case SDL_SCANCODE_V:
        return Window::Key::V;
    case SDL_SCANCODE_W:
        return Window::Key::W;
    case SDL_SCANCODE_X:
        return Window::Key::X;
    case SDL_SCANCODE_Y:
        return Window::Key::Y;
    case SDL_SCANCODE_Z:
        return Window::Key::Z;
    case SDL_SCANCODE_1:
        return Window::Key::_1;
    case SDL_SCANCODE_2:
        return Window::Key::_2;
    case SDL_SCANCODE_3:
        return Window::Key::_3;
    case SDL_SCANCODE_4:
        return Window::Key::_4;
    case SDL_SCANCODE_5:
        return Window::Key::_5;
    case SDL_SCANCODE_6:
        return Window::Key::_6;
    case SDL_SCANCODE_7:
        return Window::Key::_7;
    case SDL_SCANCODE_8:
        return Window::Key::_8;
    case SDL_SCANCODE_9:
        return Window::Key::_9;
    case SDL_SCANCODE_0:
        return Window::Key::_0;
    case SDL_SCANCODE_RETURN:
        return Window::Key::ENTER;
    case SDL_SCANCODE_ESCAPE:
        return Window::Key::ESCAPE;
    case SDL_SCANCODE_BACKSPACE:
        return Window::Key::BACKSPACE;
    case SDL_SCANCODE_TAB:
        return Window::Key::TAB;
    case SDL_SCANCODE_SPACE:
        return Window::Key::SPACE;
    case SDL_SCANCODE_MINUS:
        return Window::Key::MINUS;
    case SDL_SCANCODE_EQUALS:
        return Window::Key::EQUAL;
    case SDL_SCANCODE_LEFTBRACKET:
        return Window::Key::LEFT_BRACKET;
    case SDL_SCANCODE_RIGHTBRACKET:
        return Window::Key::RIGHT_BRACKET;
    case SDL_SCANCODE_BACKSLASH:
        return Window::Key::BACKSLASH;
    case SDL_SCANCODE_NONUSHASH:
        return Window::Key::SLASH;
    case SDL_SCANCODE_SEMICOLON:
        return Window::Key::SEMICOLON;
    case SDL_SCANCODE_APOSTROPHE:
        return Window::Key::APOSTROPHE;
    case SDL_SCANCODE_GRAVE:
        return Window::Key::GRAVE_ACCENT;
    case SDL_SCANCODE_COMMA:
        return Window::Key::COMMA;
    case SDL_SCANCODE_PERIOD:
        return Window::Key::PERIOD;
    case SDL_SCANCODE_SLASH:
        return Window::Key::SLASH;
    case SDL_SCANCODE_CAPSLOCK:
        return Window::Key::CAPS_LOCK;
    case SDL_SCANCODE_F1:
        return Window::Key::F1;
    case SDL_SCANCODE_F2:
        return Window::Key::F2;
    case SDL_SCANCODE_F3:
        return Window::Key::F3;
    case SDL_SCANCODE_F4:
        return Window::Key::F4;
    case SDL_SCANCODE_F5:
        return Window::Key::F5;
    case SDL_SCANCODE_F6:
        return Window::Key::F6;
    case SDL_SCANCODE_F7:
        return Window::Key::F7;
    case SDL_SCANCODE_F8:
        return Window::Key::F8;
    case SDL_SCANCODE_F9:
        return Window::Key::F9;
    case SDL_SCANCODE_F10:
        return Window::Key::F10;
    case SDL_SCANCODE_F11:
        return Window::Key::F11;
    case SDL_SCANCODE_F12:
        return Window::Key::F12;
    case SDL_SCANCODE_PRINTSCREEN:
        return Window::Key::PRINT_SCREEN;
    case SDL_SCANCODE_SCROLLLOCK:
        return Window::Key::SCROLL_LOCK;
    case SDL_SCANCODE_PAUSE:
        return Window::Key::PAUSE;
    case SDL_SCANCODE_INSERT:
        return Window::Key::INSERT;
    case SDL_SCANCODE_HOME:
        return Window::Key::HOME;
    case SDL_SCANCODE_PAGEUP:
        return Window::Key::PAGE_UP;
    case SDL_SCANCODE_DELETE:
        return Window::Key::DELETE;
    case SDL_SCANCODE_END:
        return Window::Key::END;
    case SDL_SCANCODE_PAGEDOWN:
        return Window::Key::PAGE_DOWN;
    case SDL_SCANCODE_RIGHT:
        return Window::Key::RIGHT;
    case SDL_SCANCODE_LEFT:
        return Window::Key::LEFT;
    case SDL_SCANCODE_DOWN:
        return Window::Key::DOWN;
    case SDL_SCANCODE_UP:
        return Window::Key::UP;
    case SDL_SCANCODE_NUMLOCKCLEAR:
        return Window::Key::NUM_LOCK;
    case SDL_SCANCODE_KP_DIVIDE:
        return Window::Key::KP_DIVIDE;
    case SDL_SCANCODE_KP_MULTIPLY:
        return Window::Key::KP_MULTIPLY;
    case SDL_SCANCODE_KP_MINUS:
        return Window::Key::KP_SUBTRACT;
    case SDL_SCANCODE_KP_PLUS:
        return Window::Key::KP_ADD;
    case SDL_SCANCODE_KP_ENTER:
        return Window::Key::KP_ENTER;
    case SDL_SCANCODE_KP_1:
        return Window::Key::KP_1;
    case SDL_SCANCODE_KP_2:
        return Window::Key::KP_2;
    case SDL_SCANCODE_KP_3:
        return Window::Key::KP_3;
    case SDL_SCANCODE_KP_4:
        return Window::Key::KP_4;
    case SDL_SCANCODE_KP_5:
        return Window::Key::KP_5;
    case SDL_SCANCODE_KP_6:
        return Window::Key::KP_6;
    case SDL_SCANCODE_KP_7:
        return Window::Key::KP_7;
    case SDL_SCANCODE_KP_8:
        return Window::Key::KP_8;
    case SDL_SCANCODE_KP_9:
        return Window::Key::KP_9;
    case SDL_SCANCODE_KP_0:
        return Window::Key::KP_0;
    case SDL_SCANCODE_KP_PERIOD:
        return Window::Key::KP_DIVIDE;
    case SDL_SCANCODE_NONUSBACKSLASH:
        return Window::Key::BACKSLASH;
    case SDL_SCANCODE_APPLICATION:
        return std::nullopt;
    case SDL_SCANCODE_POWER:
        return std::nullopt;
    case SDL_SCANCODE_KP_EQUALS:
        return Window::Key::KP_EQUAL;
    case SDL_SCANCODE_F13:
        return Window::Key::F13;
    case SDL_SCANCODE_F14:
        return Window::Key::F14;
    case SDL_SCANCODE_F15:
        return Window::Key::F15;
    case SDL_SCANCODE_F16:
        return Window::Key::F16;
    case SDL_SCANCODE_F17:
        return Window::Key::F17;
    case SDL_SCANCODE_F18:
        return Window::Key::F18;
    case SDL_SCANCODE_F19:
        return Window::Key::F19;
    case SDL_SCANCODE_F20:
        return Window::Key::F20;
    case SDL_SCANCODE_F21:
        return Window::Key::F21;
    case SDL_SCANCODE_F22:
        return Window::Key::F22;
    case SDL_SCANCODE_F23:
        return Window::Key::F23;
    case SDL_SCANCODE_F24:
        return Window::Key::F24;
    case SDL_SCANCODE_EXECUTE:
    case SDL_SCANCODE_HELP:
    case SDL_SCANCODE_MENU:
    case SDL_SCANCODE_SELECT:
    case SDL_SCANCODE_STOP:
    case SDL_SCANCODE_AGAIN:
    case SDL_SCANCODE_UNDO:
    case SDL_SCANCODE_CUT:
    case SDL_SCANCODE_COPY:
    case SDL_SCANCODE_PASTE:
    case SDL_SCANCODE_FIND:
    case SDL_SCANCODE_MUTE:
    case SDL_SCANCODE_VOLUMEUP:
    case SDL_SCANCODE_VOLUMEDOWN:
    case SDL_SCANCODE_KP_COMMA:
    case SDL_SCANCODE_KP_EQUALSAS400:
    case SDL_SCANCODE_INTERNATIONAL1:
    case SDL_SCANCODE_INTERNATIONAL2:
    case SDL_SCANCODE_INTERNATIONAL3:
    case SDL_SCANCODE_INTERNATIONAL4:
    case SDL_SCANCODE_INTERNATIONAL5:
    case SDL_SCANCODE_INTERNATIONAL6:
    case SDL_SCANCODE_INTERNATIONAL7:
    case SDL_SCANCODE_INTERNATIONAL8:
    case SDL_SCANCODE_INTERNATIONAL9:
    case SDL_SCANCODE_LANG1:
    case SDL_SCANCODE_LANG2:
    case SDL_SCANCODE_LANG3:
    case SDL_SCANCODE_LANG4:
    case SDL_SCANCODE_LANG5:
    case SDL_SCANCODE_LANG6:
    case SDL_SCANCODE_LANG7:
    case SDL_SCANCODE_LANG8:
    case SDL_SCANCODE_LANG9:
    case SDL_SCANCODE_ALTERASE:
    case SDL_SCANCODE_SYSREQ:
    case SDL_SCANCODE_CANCEL:
    case SDL_SCANCODE_CLEAR:
    case SDL_SCANCODE_PRIOR:
    case SDL_SCANCODE_RETURN2:
    case SDL_SCANCODE_SEPARATOR:
    case SDL_SCANCODE_OUT:
    case SDL_SCANCODE_OPER:
    case SDL_SCANCODE_CLEARAGAIN:
    case SDL_SCANCODE_CRSEL:
    case SDL_SCANCODE_EXSEL:
    case SDL_SCANCODE_KP_00:
    case SDL_SCANCODE_KP_000:
    case SDL_SCANCODE_THOUSANDSSEPARATOR:
    case SDL_SCANCODE_DECIMALSEPARATOR:
    case SDL_SCANCODE_CURRENCYUNIT:
    case SDL_SCANCODE_CURRENCYSUBUNIT:
    case SDL_SCANCODE_KP_LEFTPAREN:
    case SDL_SCANCODE_KP_RIGHTPAREN:
    case SDL_SCANCODE_KP_LEFTBRACE:
    case SDL_SCANCODE_KP_RIGHTBRACE:
    case SDL_SCANCODE_KP_TAB:
    case SDL_SCANCODE_KP_BACKSPACE:
    case SDL_SCANCODE_KP_A:
    case SDL_SCANCODE_KP_B:
    case SDL_SCANCODE_KP_C:
    case SDL_SCANCODE_KP_D:
    case SDL_SCANCODE_KP_E:
    case SDL_SCANCODE_KP_F:
    case SDL_SCANCODE_KP_XOR:
    case SDL_SCANCODE_KP_POWER:
    case SDL_SCANCODE_KP_PERCENT:
    case SDL_SCANCODE_KP_LESS:
    case SDL_SCANCODE_KP_GREATER:
    case SDL_SCANCODE_KP_AMPERSAND:
    case SDL_SCANCODE_KP_DBLAMPERSAND:
    case SDL_SCANCODE_KP_VERTICALBAR:
    case SDL_SCANCODE_KP_DBLVERTICALBAR:
    case SDL_SCANCODE_KP_COLON:
    case SDL_SCANCODE_KP_HASH:
    case SDL_SCANCODE_KP_SPACE:
    case SDL_SCANCODE_KP_AT:
    case SDL_SCANCODE_KP_EXCLAM:
    case SDL_SCANCODE_KP_MEMSTORE:
    case SDL_SCANCODE_KP_MEMRECALL:
    case SDL_SCANCODE_KP_MEMCLEAR:
    case SDL_SCANCODE_KP_MEMADD:
    case SDL_SCANCODE_KP_MEMSUBTRACT:
    case SDL_SCANCODE_KP_MEMMULTIPLY:
    case SDL_SCANCODE_KP_MEMDIVIDE:
    case SDL_SCANCODE_KP_PLUSMINUS:
    case SDL_SCANCODE_KP_CLEAR:
    case SDL_SCANCODE_KP_CLEARENTRY:
    case SDL_SCANCODE_KP_BINARY:
    case SDL_SCANCODE_KP_OCTAL:
    case SDL_SCANCODE_KP_DECIMAL:
    case SDL_SCANCODE_KP_HEXADECIMAL:
    case SDL_SCANCODE_LCTRL:
        return Window::Key::LEFT_CONTROL;
    case SDL_SCANCODE_LSHIFT:
        return Window::Key::LEFT_SHIFT;
    case SDL_SCANCODE_LALT:
        return Window::Key::LEFT_ALT;
    case SDL_SCANCODE_LGUI:
    case SDL_SCANCODE_RCTRL:
        return Window::Key::RIGHT_CONTROL;
    case SDL_SCANCODE_RSHIFT:
        return Window::Key::RIGHT_SHIFT;
    case SDL_SCANCODE_RALT:
        return Window::Key::RIGHT_ALT;
    case SDL_SCANCODE_RGUI:
    case SDL_SCANCODE_MODE:
    case SDL_SCANCODE_AUDIONEXT:
    case SDL_SCANCODE_AUDIOPREV:
    case SDL_SCANCODE_AUDIOSTOP:
    case SDL_SCANCODE_AUDIOPLAY:
    case SDL_SCANCODE_AUDIOMUTE:
    case SDL_SCANCODE_MEDIASELECT:
    case SDL_SCANCODE_WWW:
    case SDL_SCANCODE_MAIL:
    case SDL_SCANCODE_CALCULATOR:
    case SDL_SCANCODE_COMPUTER:
    case SDL_SCANCODE_AC_SEARCH:
    case SDL_SCANCODE_AC_HOME:
    case SDL_SCANCODE_AC_BACK:
    case SDL_SCANCODE_AC_FORWARD:
    case SDL_SCANCODE_AC_STOP:
    case SDL_SCANCODE_AC_REFRESH:
    case SDL_SCANCODE_AC_BOOKMARKS:
    case SDL_SCANCODE_BRIGHTNESSDOWN:
    case SDL_SCANCODE_BRIGHTNESSUP:
    case SDL_SCANCODE_DISPLAYSWITCH:
    case SDL_SCANCODE_KBDILLUMTOGGLE:
    case SDL_SCANCODE_KBDILLUMDOWN:
    case SDL_SCANCODE_KBDILLUMUP:
    case SDL_SCANCODE_EJECT:
    case SDL_SCANCODE_SLEEP:
    case SDL_SCANCODE_APP1:
    case SDL_SCANCODE_APP2:
    case SDL_SCANCODE_AUDIOREWIND:
    case SDL_SCANCODE_AUDIOFASTFORWARD:
    case SDL_SCANCODE_SOFTLEFT:
    case SDL_SCANCODE_SOFTRIGHT:
    case SDL_SCANCODE_CALL:
    case SDL_SCANCODE_ENDCALL:
    case SDL_NUM_SCANCODES:
        break;
    }
    return std::nullopt;
}

RawPtr<SDL_Window> createWindow(const std::string& title, std::size_t w, std::size_t h)
{
    static std::atomic_bool inited = false;
    if (!inited.exchange(true)) {
        if (::SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            std::cerr << "Failed to initialize sdl2: " << SDL_GetError() << std::endl;
            std::abort();
        }
    }

    const auto window = ::SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        numericCast<int>(w).value(),
        numericCast<int>(h).value(),
        SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Failed to create sdl2 window: " << SDL_GetError() << std::endl;
        std::abort();
    }

    return window;
}
}

class SDL2Window::Impl {
public:
    Impl(const std::string& title, std::size_t w, std::size_t h)
        : m_window(createWindow(title, w, h))
    {
    }

    glm::vec2 size() const
    {
        int w = 0, h = 0;
        ::SDL_GetWindowSize(m_window.nullable(), &w, &h);
        return {
            static_cast<float>(w),
            static_cast<float>(h)
        };
    }

    glm::vec2 mousePosition() const
    {
        if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
            return m_relativeMousePosition;
        } else {
            int x = 0, y = 0;
            ::SDL_GetMouseState(&x, &y);
            return {
                static_cast<float>(x),
                static_cast<float>(y)
            };
        }
    }

    bool isPressed(Key key) const
    {
        pollEvents();
        return m_pressedKeys[key];
    }

    bool shouldClose() const
    {
        pollEvents();
        return m_shouldClose;
    }

    [[nodiscard]] Expected<void> setCursorVisible(bool v) noexcept
    {
        if (::SDL_SetRelativeMouseMode(v ? SDL_FALSE : SDL_TRUE) < 0) {
            return unexpected(std::string("Failed to SDL_SetRelativeMouseMode: ") + ::SDL_GetError());
        }
        return {};
    }

    void setMousePosition(const glm::vec2& pos)
    {
        ::SDL_WarpMouseInWindow(m_window.nullable(), numericCast<int>(pos.x).value(), numericCast<int>(pos.y).value());
    }

    Expected<vk::SurfaceKHR> createVulkanSurface(vk::Instance i)
    {
        VkSurfaceKHR surface;
        if (!::SDL_Vulkan_CreateSurface(m_window.nullable(), i, &surface)) {
            return unexpected("surface creating error");
        }
        return surface;
    }

    std::vector<std::string> requiredVulkanExtensions() const
    {
        unsigned int count = 0;
        if (!::SDL_Vulkan_GetInstanceExtensions(m_window.nullable(), &count, nullptr)) {
            std::cerr << "Failed to get vulkan instance extensions count for sdl2: " << SDL_GetError() << std::endl;
            std::abort();
        }

        std::vector<const char*> extensions(count);
        if (!::SDL_Vulkan_GetInstanceExtensions(m_window.nullable(), &count, extensions.data())) {
            std::cerr << "Failed to get vulkan instance extensions for sdl2: " << SDL_GetError() << std::endl;
            std::abort();
        }

        std::vector<std::string> result(count);
        for (std::uint32_t i = 0; i < count; ++i) {
            result[i] = extensions[i];
        }
        return result;
    }

    ~Impl()
    {
        ::SDL_DestroyWindow(m_window.nullable());
        ::SDL_Quit();
    }

private:
    void pollEvents() const
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                m_shouldClose = true;
            } else if (event.type == SDL_KEYDOWN) {
                const auto key = windowKeyFromSDLScancode(event.key.keysym.scancode);
                if (key) {
                    m_pressedKeys[*key] = true;
                } else {
                    std::cerr << "Unknown SDL scancode: " << event.key.keysym.scancode << std::endl;
                }
            } else if (event.type == SDL_KEYUP) {
                const auto key = windowKeyFromSDLScancode(event.key.keysym.scancode);
                if (key) {
                    m_pressedKeys[*key] = false;
                } else {
                    std::cerr << "Unknown SDL scancode: " << event.key.keysym.scancode << std::endl;
                }
            } else if (event.type == SDL_MOUSEMOTION && SDL_GetRelativeMouseMode() == SDL_TRUE) {
                int x = 0, y = 0;
                ::SDL_GetRelativeMouseState(&x, &y);
                m_relativeMousePosition += glm::vec2 {
                    static_cast<float>(x),
                    static_cast<float>(y)
                };
            }
        }
    }

private:
    RawPtr<SDL_Window> m_window;
    mutable std::atomic_bool m_shouldClose = false;
    mutable std::map<Key, bool> m_pressedKeys;
    mutable glm::vec2 m_relativeMousePosition;
};

SDL2Window::SDL2Window(const std::string& title, std::size_t w, std::size_t h)
    : m_impl(std::make_unique<Impl>(title, w, h))
{
}

SDL2Window::~SDL2Window() = default;

bool SDL2Window::isPressed(Key key) const
{
    return m_impl->isPressed(key);
}

bool SDL2Window::shouldClose() const
{
    return m_impl->shouldClose();
}

glm::vec2 SDL2Window::size() const
{
    return m_impl->size();
}

glm::vec2 SDL2Window::mousePosition() const
{
    return m_impl->mousePosition();
}

void SDL2Window::setMousePosition(const glm::vec2& pos)
{
    m_impl->setMousePosition(pos);
}

Expected<void> SDL2Window::setCursorVisible(bool v) noexcept
{
    return m_impl->setCursorVisible(v);
}

Expected<vk::SurfaceKHR> SDL2Window::createVulkanSurface(vk::Instance i)
{
    return m_impl->createVulkanSurface(i);
}

std::vector<std::string> SDL2Window::requiredVulkanExtensions() const
{
    return m_impl->requiredVulkanExtensions();
}

}
