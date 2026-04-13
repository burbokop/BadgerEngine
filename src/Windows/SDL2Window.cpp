#include "SDL2Window.h"

#include "../Utils/NumericCast.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <atomic>
#include <iostream>
#include <map>

namespace BadgerEngine {

namespace {

std::optional<InputProvider::Key> inputProviderKeyFromSDLScancode(SDL_Scancode scancode)
{
    switch (scancode) {
    case SDL_SCANCODE_UNKNOWN:
        return std::nullopt;
    case SDL_SCANCODE_A:
        return InputProvider::Key::A;
    case SDL_SCANCODE_B:
        return InputProvider::Key::B;
    case SDL_SCANCODE_C:
        return InputProvider::Key::C;
    case SDL_SCANCODE_D:
        return InputProvider::Key::D;
    case SDL_SCANCODE_E:
        return InputProvider::Key::E;
    case SDL_SCANCODE_F:
        return InputProvider::Key::F;
    case SDL_SCANCODE_G:
        return InputProvider::Key::G;
    case SDL_SCANCODE_H:
        return InputProvider::Key::H;
    case SDL_SCANCODE_I:
        return InputProvider::Key::I;
    case SDL_SCANCODE_J:
        return InputProvider::Key::J;
    case SDL_SCANCODE_K:
        return InputProvider::Key::K;
    case SDL_SCANCODE_L:
        return InputProvider::Key::L;
    case SDL_SCANCODE_M:
        return InputProvider::Key::M;
    case SDL_SCANCODE_N:
        return InputProvider::Key::N;
    case SDL_SCANCODE_O:
        return InputProvider::Key::O;
    case SDL_SCANCODE_P:
        return InputProvider::Key::P;
    case SDL_SCANCODE_Q:
        return InputProvider::Key::Q;
    case SDL_SCANCODE_R:
        return InputProvider::Key::R;
    case SDL_SCANCODE_S:
        return InputProvider::Key::S;
    case SDL_SCANCODE_T:
        return InputProvider::Key::T;
    case SDL_SCANCODE_U:
        return InputProvider::Key::U;
    case SDL_SCANCODE_V:
        return InputProvider::Key::V;
    case SDL_SCANCODE_W:
        return InputProvider::Key::W;
    case SDL_SCANCODE_X:
        return InputProvider::Key::X;
    case SDL_SCANCODE_Y:
        return InputProvider::Key::Y;
    case SDL_SCANCODE_Z:
        return InputProvider::Key::Z;
    case SDL_SCANCODE_1:
        return InputProvider::Key::_1;
    case SDL_SCANCODE_2:
        return InputProvider::Key::_2;
    case SDL_SCANCODE_3:
        return InputProvider::Key::_3;
    case SDL_SCANCODE_4:
        return InputProvider::Key::_4;
    case SDL_SCANCODE_5:
        return InputProvider::Key::_5;
    case SDL_SCANCODE_6:
        return InputProvider::Key::_6;
    case SDL_SCANCODE_7:
        return InputProvider::Key::_7;
    case SDL_SCANCODE_8:
        return InputProvider::Key::_8;
    case SDL_SCANCODE_9:
        return InputProvider::Key::_9;
    case SDL_SCANCODE_0:
        return InputProvider::Key::_0;
    case SDL_SCANCODE_RETURN:
        return InputProvider::Key::Enter;
    case SDL_SCANCODE_ESCAPE:
        return InputProvider::Key::Escape;
    case SDL_SCANCODE_BACKSPACE:
        return InputProvider::Key::Backspace;
    case SDL_SCANCODE_TAB:
        return InputProvider::Key::Tab;
    case SDL_SCANCODE_SPACE:
        return InputProvider::Key::Space;
    case SDL_SCANCODE_MINUS:
        return InputProvider::Key::Minus;
    case SDL_SCANCODE_EQUALS:
        return InputProvider::Key::Equal;
    case SDL_SCANCODE_LEFTBRACKET:
        return InputProvider::Key::LeftBracket;
    case SDL_SCANCODE_RIGHTBRACKET:
        return InputProvider::Key::RightBracket;
    case SDL_SCANCODE_BACKSLASH:
        return InputProvider::Key::Backslash;
    case SDL_SCANCODE_NONUSHASH:
        return InputProvider::Key::Slash;
    case SDL_SCANCODE_SEMICOLON:
        return InputProvider::Key::Semicolon;
    case SDL_SCANCODE_APOSTROPHE:
        return InputProvider::Key::Apostrophe;
    case SDL_SCANCODE_GRAVE:
        return InputProvider::Key::GraveAccent;
    case SDL_SCANCODE_COMMA:
        return InputProvider::Key::Comma;
    case SDL_SCANCODE_PERIOD:
        return InputProvider::Key::Period;
    case SDL_SCANCODE_SLASH:
        return InputProvider::Key::Slash;
    case SDL_SCANCODE_CAPSLOCK:
        return InputProvider::Key::CapsLock;
    case SDL_SCANCODE_F1:
        return InputProvider::Key::F1;
    case SDL_SCANCODE_F2:
        return InputProvider::Key::F2;
    case SDL_SCANCODE_F3:
        return InputProvider::Key::F3;
    case SDL_SCANCODE_F4:
        return InputProvider::Key::F4;
    case SDL_SCANCODE_F5:
        return InputProvider::Key::F5;
    case SDL_SCANCODE_F6:
        return InputProvider::Key::F6;
    case SDL_SCANCODE_F7:
        return InputProvider::Key::F7;
    case SDL_SCANCODE_F8:
        return InputProvider::Key::F8;
    case SDL_SCANCODE_F9:
        return InputProvider::Key::F9;
    case SDL_SCANCODE_F10:
        return InputProvider::Key::F10;
    case SDL_SCANCODE_F11:
        return InputProvider::Key::F11;
    case SDL_SCANCODE_F12:
        return InputProvider::Key::F12;
    case SDL_SCANCODE_PRINTSCREEN:
        return InputProvider::Key::PrintScreen;
    case SDL_SCANCODE_SCROLLLOCK:
        return InputProvider::Key::ScrollLock;
    case SDL_SCANCODE_PAUSE:
        return InputProvider::Key::Pause;
    case SDL_SCANCODE_INSERT:
        return InputProvider::Key::Insert;
    case SDL_SCANCODE_HOME:
        return InputProvider::Key::Home;
    case SDL_SCANCODE_PAGEUP:
        return InputProvider::Key::PageUp;
    case SDL_SCANCODE_DELETE:
        return InputProvider::Key::Delete;
    case SDL_SCANCODE_END:
        return InputProvider::Key::End;
    case SDL_SCANCODE_PAGEDOWN:
        return InputProvider::Key::PageDown;
    case SDL_SCANCODE_RIGHT:
        return InputProvider::Key::Right;
    case SDL_SCANCODE_LEFT:
        return InputProvider::Key::Left;
    case SDL_SCANCODE_DOWN:
        return InputProvider::Key::Down;
    case SDL_SCANCODE_UP:
        return InputProvider::Key::Up;
    case SDL_SCANCODE_NUMLOCKCLEAR:
        return InputProvider::Key::NumLock;
    case SDL_SCANCODE_KP_DIVIDE:
        return InputProvider::Key::KPDivide;
    case SDL_SCANCODE_KP_MULTIPLY:
        return InputProvider::Key::KPMultiply;
    case SDL_SCANCODE_KP_MINUS:
        return InputProvider::Key::KPSubtract;
    case SDL_SCANCODE_KP_PLUS:
        return InputProvider::Key::KPAdd;
    case SDL_SCANCODE_KP_ENTER:
        return InputProvider::Key::KPEnter;
    case SDL_SCANCODE_KP_1:
        return InputProvider::Key::KP1;
    case SDL_SCANCODE_KP_2:
        return InputProvider::Key::KP2;
    case SDL_SCANCODE_KP_3:
        return InputProvider::Key::KP3;
    case SDL_SCANCODE_KP_4:
        return InputProvider::Key::KP4;
    case SDL_SCANCODE_KP_5:
        return InputProvider::Key::KP5;
    case SDL_SCANCODE_KP_6:
        return InputProvider::Key::KP6;
    case SDL_SCANCODE_KP_7:
        return InputProvider::Key::KP7;
    case SDL_SCANCODE_KP_8:
        return InputProvider::Key::KP8;
    case SDL_SCANCODE_KP_9:
        return InputProvider::Key::KP9;
    case SDL_SCANCODE_KP_0:
        return InputProvider::Key::KP0;
    case SDL_SCANCODE_KP_PERIOD:
        return InputProvider::Key::KPDivide;
    case SDL_SCANCODE_NONUSBACKSLASH:
        return InputProvider::Key::Backslash;
    case SDL_SCANCODE_APPLICATION:
        return std::nullopt;
    case SDL_SCANCODE_POWER:
        return std::nullopt;
    case SDL_SCANCODE_KP_EQUALS:
        return InputProvider::Key::KPEqual;
    case SDL_SCANCODE_F13:
        return InputProvider::Key::F13;
    case SDL_SCANCODE_F14:
        return InputProvider::Key::F14;
    case SDL_SCANCODE_F15:
        return InputProvider::Key::F15;
    case SDL_SCANCODE_F16:
        return InputProvider::Key::F16;
    case SDL_SCANCODE_F17:
        return InputProvider::Key::F17;
    case SDL_SCANCODE_F18:
        return InputProvider::Key::F18;
    case SDL_SCANCODE_F19:
        return InputProvider::Key::F19;
    case SDL_SCANCODE_F20:
        return InputProvider::Key::F20;
    case SDL_SCANCODE_F21:
        return InputProvider::Key::F21;
    case SDL_SCANCODE_F22:
        return InputProvider::Key::F22;
    case SDL_SCANCODE_F23:
        return InputProvider::Key::F23;
    case SDL_SCANCODE_F24:
        return InputProvider::Key::F24;
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
        return InputProvider::Key::LeftControl;
    case SDL_SCANCODE_LSHIFT:
        return InputProvider::Key::LeftShift;
    case SDL_SCANCODE_LALT:
        return InputProvider::Key::LeftAlt;
    case SDL_SCANCODE_LGUI:
    case SDL_SCANCODE_RCTRL:
        return InputProvider::Key::RightControl;
    case SDL_SCANCODE_RSHIFT:
        return InputProvider::Key::RightShift;
    case SDL_SCANCODE_RALT:
        return InputProvider::Key::RightAlt;
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

    bool pressed(Key key) const
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
                const auto key = inputProviderKeyFromSDLScancode(event.key.keysym.scancode);
                if (key) {
                    m_pressedKeys[*key] = true;
                } else {
                    std::cerr << "Unknown SDL scancode: " << event.key.keysym.scancode << std::endl;
                }
            } else if (event.type == SDL_KEYUP) {
                const auto key = inputProviderKeyFromSDLScancode(event.key.keysym.scancode);
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

bool SDL2Window::pressed(Key key) const
{
    return m_impl->pressed(key);
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
