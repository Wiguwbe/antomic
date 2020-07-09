/*
   Copyright 2020 Alexandre Pires (c.alexandre.pires@gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#ifdef ENGINE_SDL_PLATFORM

#include "Platform/SDL/PlatformSDL.h"
#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include <SDL2/SDL_syswm.h>
#include "bx/bx.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

static uint8_t translateKeyModifiers(uint16_t _sdl)
{
    uint8_t modifiers = 0;
    modifiers |= _sdl & KMOD_LALT ? Engine::Modifier::LeftAlt : 0;
    modifiers |= _sdl & KMOD_RALT ? Engine::Modifier::RightAlt : 0;
    modifiers |= _sdl & KMOD_LCTRL ? Engine::Modifier::LeftCtrl : 0;
    modifiers |= _sdl & KMOD_RCTRL ? Engine::Modifier::RightCtrl : 0;
    modifiers |= _sdl & KMOD_LSHIFT ? Engine::Modifier::LeftShift : 0;
    modifiers |= _sdl & KMOD_RSHIFT ? Engine::Modifier::RightShift : 0;
    modifiers |= _sdl & KMOD_LGUI ? Engine::Modifier::LeftMeta : 0;
    modifiers |= _sdl & KMOD_RGUI ? Engine::Modifier::RightMeta : 0;
    return modifiers;
}

static uint8_t translateKeyModifierPress(uint16_t _key)
{
    uint8_t modifier;
    switch (_key)
    {
    case SDL_SCANCODE_LALT:
    {
        modifier = Engine::Modifier::LeftAlt;
    }
    break;
    case SDL_SCANCODE_RALT:
    {
        modifier = Engine::Modifier::RightAlt;
    }
    break;
    case SDL_SCANCODE_LCTRL:
    {
        modifier = Engine::Modifier::LeftCtrl;
    }
    break;
    case SDL_SCANCODE_RCTRL:
    {
        modifier = Engine::Modifier::RightCtrl;
    }
    break;
    case SDL_SCANCODE_LSHIFT:
    {
        modifier = Engine::Modifier::LeftShift;
    }
    break;
    case SDL_SCANCODE_RSHIFT:
    {
        modifier = Engine::Modifier::RightShift;
    }
    break;
    case SDL_SCANCODE_LGUI:
    {
        modifier = Engine::Modifier::LeftMeta;
    }
    break;
    case SDL_SCANCODE_RGUI:
    {
        modifier = Engine::Modifier::RightMeta;
    }
    break;
    default:
    {
        modifier = 0;
    }
    break;
    }

    return modifier;
}

static uint8_t s_translateKey[256];

static void initTranslateKey(uint16_t _sdl, Engine::Key::Enum _key)
{
    BX_CHECK(_sdl < BX_COUNTOF(s_translateKey), "Out of bounds %d.", _sdl);
    s_translateKey[_sdl & 0xff] = (uint8_t)_key;
}

static Engine::Key::Enum translateKey(SDL_Scancode _sdl)
{
    return (Engine::Key::Enum)s_translateKey[_sdl & 0xff];
}

// static uint8_t s_translateGamepad[256];

// static void initTranslateGamepad(uint8_t _sdl, Engine::Key::Enum _button)
// {
//     s_translateGamepad[_sdl] = _button;
// }

// static Engine::Key::Enum translateGamepad(uint8_t _sdl)
// {
//     return Engine::Key::Enum(s_translateGamepad[_sdl]);
// }

// static uint8_t s_translateGamepadAxis[256];

// static void initTranslateGamepadAxis(uint8_t _sdl, Engine::GamepadAxis::Enum _axis)
// {
//     s_translateGamepadAxis[_sdl] = uint8_t(_axis);
// }

// static Engine::GamepadAxis::Enum translateGamepadAxis(uint8_t _sdl)
// {
//     return Engine::GamepadAxis::Enum(s_translateGamepadAxis[_sdl]);
// }

namespace Engine
{
    SDL_Window *PlatformSDL::s_SDLWindow = nullptr;

    PlatformSDL::PlatformSDL(): m_Handler(nullptr)
    {
    }

    PlatformSDL::~PlatformSDL()
    {
    }

    bool PlatformSDL::Init()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            ENGINE_TRACE("Error initializing SDL");
            return false;
        }

        return true;
    }

    void PlatformSDL::Shutdown()
    {
        SDL_Quit();
    }

    bool PlatformSDL::CreateWindow(uint32_t width, uint32_t height, std::string name)
    {
        m_Width = width;
        m_Height = height;

        s_SDLWindow = SDL_CreateWindow(
            name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

        if (!s_SDLWindow)
        {
            ENGINE_TRACE("Error creating SDL window: {0},{1}", width, height);
            SDL_Quit();
            return false;
        }

        SDL_SysWMinfo wmi;
        SDL_VERSION(&wmi.version);

        if (!SDL_GetWindowWMInfo(s_SDLWindow, &wmi))
        {
            ENGINE_TRACE("Error retrieving window information");
            SDL_DestroyWindow(s_SDLWindow);
            SDL_Quit();
            return false;
        }

        bgfx::PlatformData pd;
        pd.ndt = wmi.info.x11.display;
        pd.nwh = (void *)(uintptr_t)wmi.info.x11.window;
        bgfx::setPlatformData(pd);

        bx::memSet(s_translateKey, 0, sizeof(s_translateKey));
        initTranslateKey(SDL_SCANCODE_ESCAPE, Key::Esc);
        initTranslateKey(SDL_SCANCODE_RETURN, Key::Return);
        initTranslateKey(SDL_SCANCODE_TAB, Key::Tab);
        initTranslateKey(SDL_SCANCODE_BACKSPACE, Key::Backspace);
        initTranslateKey(SDL_SCANCODE_SPACE, Key::Space);
        initTranslateKey(SDL_SCANCODE_UP, Key::Up);
        initTranslateKey(SDL_SCANCODE_DOWN, Key::Down);
        initTranslateKey(SDL_SCANCODE_LEFT, Key::Left);
        initTranslateKey(SDL_SCANCODE_RIGHT, Key::Right);
        initTranslateKey(SDL_SCANCODE_PAGEUP, Key::PageUp);
        initTranslateKey(SDL_SCANCODE_PAGEDOWN, Key::PageDown);
        initTranslateKey(SDL_SCANCODE_HOME, Key::Home);
        initTranslateKey(SDL_SCANCODE_END, Key::End);
        initTranslateKey(SDL_SCANCODE_PRINTSCREEN, Key::Print);
        initTranslateKey(SDL_SCANCODE_KP_PLUS, Key::Plus);
        initTranslateKey(SDL_SCANCODE_EQUALS, Key::Plus);
        initTranslateKey(SDL_SCANCODE_KP_MINUS, Key::Minus);
        initTranslateKey(SDL_SCANCODE_MINUS, Key::Minus);
        initTranslateKey(SDL_SCANCODE_GRAVE, Key::Tilde);
        initTranslateKey(SDL_SCANCODE_KP_COMMA, Key::Comma);
        initTranslateKey(SDL_SCANCODE_COMMA, Key::Comma);
        initTranslateKey(SDL_SCANCODE_KP_PERIOD, Key::Period);
        initTranslateKey(SDL_SCANCODE_PERIOD, Key::Period);
        initTranslateKey(SDL_SCANCODE_SLASH, Key::Slash);
        initTranslateKey(SDL_SCANCODE_F1, Key::F1);
        initTranslateKey(SDL_SCANCODE_F2, Key::F2);
        initTranslateKey(SDL_SCANCODE_F3, Key::F3);
        initTranslateKey(SDL_SCANCODE_F4, Key::F4);
        initTranslateKey(SDL_SCANCODE_F5, Key::F5);
        initTranslateKey(SDL_SCANCODE_F6, Key::F6);
        initTranslateKey(SDL_SCANCODE_F7, Key::F7);
        initTranslateKey(SDL_SCANCODE_F8, Key::F8);
        initTranslateKey(SDL_SCANCODE_F9, Key::F9);
        initTranslateKey(SDL_SCANCODE_F10, Key::F10);
        initTranslateKey(SDL_SCANCODE_F11, Key::F11);
        initTranslateKey(SDL_SCANCODE_F12, Key::F12);
        initTranslateKey(SDL_SCANCODE_KP_0, Key::NumPad0);
        initTranslateKey(SDL_SCANCODE_KP_1, Key::NumPad1);
        initTranslateKey(SDL_SCANCODE_KP_2, Key::NumPad2);
        initTranslateKey(SDL_SCANCODE_KP_3, Key::NumPad3);
        initTranslateKey(SDL_SCANCODE_KP_4, Key::NumPad4);
        initTranslateKey(SDL_SCANCODE_KP_5, Key::NumPad5);
        initTranslateKey(SDL_SCANCODE_KP_6, Key::NumPad6);
        initTranslateKey(SDL_SCANCODE_KP_7, Key::NumPad7);
        initTranslateKey(SDL_SCANCODE_KP_8, Key::NumPad8);
        initTranslateKey(SDL_SCANCODE_KP_9, Key::NumPad9);
        initTranslateKey(SDL_SCANCODE_KP_ENTER, Key::NumPadEnter);
        initTranslateKey(SDL_SCANCODE_0, Key::Key0);
        initTranslateKey(SDL_SCANCODE_1, Key::Key1);
        initTranslateKey(SDL_SCANCODE_2, Key::Key2);
        initTranslateKey(SDL_SCANCODE_3, Key::Key3);
        initTranslateKey(SDL_SCANCODE_4, Key::Key4);
        initTranslateKey(SDL_SCANCODE_5, Key::Key5);
        initTranslateKey(SDL_SCANCODE_6, Key::Key6);
        initTranslateKey(SDL_SCANCODE_7, Key::Key7);
        initTranslateKey(SDL_SCANCODE_8, Key::Key8);
        initTranslateKey(SDL_SCANCODE_9, Key::Key9);
        initTranslateKey(SDL_SCANCODE_A, Key::KeyA);
        initTranslateKey(SDL_SCANCODE_B, Key::KeyB);
        initTranslateKey(SDL_SCANCODE_C, Key::KeyC);
        initTranslateKey(SDL_SCANCODE_D, Key::KeyD);
        initTranslateKey(SDL_SCANCODE_E, Key::KeyE);
        initTranslateKey(SDL_SCANCODE_F, Key::KeyF);
        initTranslateKey(SDL_SCANCODE_G, Key::KeyG);
        initTranslateKey(SDL_SCANCODE_H, Key::KeyH);
        initTranslateKey(SDL_SCANCODE_I, Key::KeyI);
        initTranslateKey(SDL_SCANCODE_J, Key::KeyJ);
        initTranslateKey(SDL_SCANCODE_K, Key::KeyK);
        initTranslateKey(SDL_SCANCODE_L, Key::KeyL);
        initTranslateKey(SDL_SCANCODE_M, Key::KeyM);
        initTranslateKey(SDL_SCANCODE_N, Key::KeyN);
        initTranslateKey(SDL_SCANCODE_O, Key::KeyO);
        initTranslateKey(SDL_SCANCODE_P, Key::KeyP);
        initTranslateKey(SDL_SCANCODE_Q, Key::KeyQ);
        initTranslateKey(SDL_SCANCODE_R, Key::KeyR);
        initTranslateKey(SDL_SCANCODE_S, Key::KeyS);
        initTranslateKey(SDL_SCANCODE_T, Key::KeyT);
        initTranslateKey(SDL_SCANCODE_U, Key::KeyU);
        initTranslateKey(SDL_SCANCODE_V, Key::KeyV);
        initTranslateKey(SDL_SCANCODE_W, Key::KeyW);
        initTranslateKey(SDL_SCANCODE_X, Key::KeyX);
        initTranslateKey(SDL_SCANCODE_Y, Key::KeyY);
        initTranslateKey(SDL_SCANCODE_Z, Key::KeyZ);

        return true;
    }

    void PlatformSDL::DestroyWindow()
    {
        SDL_DestroyWindow(s_SDLWindow);
        SDL_Quit();
    }

    void PlatformSDL::ProcessWindowEvents()
    {
        SDL_Event currentEvent;
        while (SDL_PollEvent(&currentEvent))
        {
            switch (currentEvent.type)
            {
            case SDL_WINDOWEVENT:
                switch (currentEvent.window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                {
                    WindowResizeEvent event(currentEvent.window.data1, currentEvent.window.data2);
                    m_Handler(event);
                }
                break;
                case SDL_WINDOWEVENT_CLOSE:
                {
                    WindowCloseEvent event;
                    m_Handler(event);
                }
                break;
                default:
                    break;
                }
                continue;
            case SDL_MOUSEMOTION:
            {
                const SDL_MouseMotionEvent &mev = currentEvent.motion;
                MouseMovedEvent event(mev.x, mev.y);
                m_Handler(event);
            }
            break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            {
                const SDL_MouseButtonEvent &mev = currentEvent.button;
                MouseButton::Enum button;
                switch (mev.button)
                {
                default:
                case SDL_BUTTON_LEFT:
                    button = MouseButton::Left;
                    break;
                case SDL_BUTTON_MIDDLE:
                    button = MouseButton::Middle;
                    break;
                case SDL_BUTTON_RIGHT:
                    button = MouseButton::Right;
                    break;
                }

                switch (mev.state)
                {
                default:
                case SDL_PRESSED:
                {
                    MouseButtonPressedEvent event(button);
                    m_Handler(event);
                }
                break;
                case SDL_RELEASED:
                {
                    MouseButtonReleasedEvent event(button);
                    m_Handler(event);
                }
                break;
                }
            }
            break;
            case SDL_MOUSEWHEEL:
            {
                const SDL_MouseWheelEvent &mev = currentEvent.wheel;
                MouseScrolledEvent event(mev.x, mev.y);
                m_Handler(event);
            }
            break;
            case SDL_KEYDOWN:
            {
                const SDL_KeyboardEvent &kev = currentEvent.key;
                uint8_t modifiers = translateKeyModifiers(kev.keysym.mod);
                Key::Enum key = translateKey(kev.keysym.scancode);

#if ENGINE_DEBUG
                ENGINE_INFO("SDL scancode {0}, key {1}, name {2}, key name {3}", kev.keysym.scancode, key, SDL_GetScancodeName(kev.keysym.scancode), SDL_GetKeyName(kev.keysym.scancode));
#endif

                /// If you only press (e.g.) 'shift' and nothing else, then key == 'shift', modifier == 0.
                /// Further along, pressing 'shift' + 'ctrl' would be: key == 'shift', modifier == 'ctrl.
                if (0 == key && 0 == modifiers)
                {
                    modifiers = translateKeyModifierPress(kev.keysym.scancode);
                }

                KeyPressedEvent event(key, modifiers, kev.repeat);
                m_Handler(event);
            }
            break;

            case SDL_KEYUP:
            {
                const SDL_KeyboardEvent &kev = currentEvent.key;
                uint8_t modifiers = translateKeyModifiers(kev.keysym.mod);
                Key::Enum key = translateKey(kev.keysym.scancode);
                KeyReleasedEvent event(key, modifiers);
                m_Handler(event);
            }
            break;
            default:
                break;
            }
        }
    }

    void PlatformSDL::ToggleFullscreen()
    {
    }

    void PlatformSDL::SetMouseLock(bool lock)
    {
    }

} // namespace Engine

#endif