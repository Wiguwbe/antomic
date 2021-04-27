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
#ifdef ANTOMIC_SDL_PLATFORM
#include "Platform/SDL/Input.h"
#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "SDL2/SDL_syswm.h"

static uint8_t translateKeyModifiers(uint16_t _sdl)
{
    uint8_t modifiers = 0;
    modifiers |= _sdl & KMOD_LALT ? Antomic::Modifier::LeftAlt : 0;
    modifiers |= _sdl & KMOD_RALT ? Antomic::Modifier::RightAlt : 0;
    modifiers |= _sdl & KMOD_LCTRL ? Antomic::Modifier::LeftCtrl : 0;
    modifiers |= _sdl & KMOD_RCTRL ? Antomic::Modifier::RightCtrl : 0;
    modifiers |= _sdl & KMOD_LSHIFT ? Antomic::Modifier::LeftShift : 0;
    modifiers |= _sdl & KMOD_RSHIFT ? Antomic::Modifier::RightShift : 0;
    modifiers |= _sdl & KMOD_LGUI ? Antomic::Modifier::LeftMeta : 0;
    modifiers |= _sdl & KMOD_RGUI ? Antomic::Modifier::RightMeta : 0;
    return modifiers;
}

static uint8_t translateKeyModifierPress(uint16_t _key)
{
    uint8_t modifier;
    switch (_key)
    {
    case SDL_SCANCODE_LALT:
    {
        modifier = Antomic::Modifier::LeftAlt;
    }
    break;
    case SDL_SCANCODE_RALT:
    {
        modifier = Antomic::Modifier::RightAlt;
    }
    break;
    case SDL_SCANCODE_LCTRL:
    {
        modifier = Antomic::Modifier::LeftCtrl;
    }
    break;
    case SDL_SCANCODE_RCTRL:
    {
        modifier = Antomic::Modifier::RightCtrl;
    }
    break;
    case SDL_SCANCODE_LSHIFT:
    {
        modifier = Antomic::Modifier::LeftShift;
    }
    break;
    case SDL_SCANCODE_RSHIFT:
    {
        modifier = Antomic::Modifier::RightShift;
    }
    break;
    case SDL_SCANCODE_LGUI:
    {
        modifier = Antomic::Modifier::LeftMeta;
    }
    break;
    case SDL_SCANCODE_RGUI:
    {
        modifier = Antomic::Modifier::RightMeta;
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

static uint8_t stranslateKey[256];

static void initTranslateKey(uint16_t _sdl, Antomic::Key::Enum _key)
{
    stranslateKey[_sdl & 0xff] = (uint8_t)_key;
}

static Antomic::Key::Enum translateKey(SDL_Scancode _sdl)
{
    return (Antomic::Key::Enum)stranslateKey[_sdl & 0xff];
}

// static uint8_t stranslateGamepad[256];

// static void initTranslateGamepad(uint8_t _sdl, Antomic::Key::Enum _button)
// {
//     stranslateGamepad[_sdl] = _button;
// }

// static Antomic::Key::Enum translateGamepad(uint8_t _sdl)
// {
//     return Antomic::Key::Enum(stranslateGamepad[_sdl]);
// }

// static uint8_t stranslateGamepadAxis[256];

// static void initTranslateGamepadAxis(uint8_t _sdl, Antomic::GamepadAxis::Enum _axis)
// {
//     stranslateGamepadAxis[_sdl] = uint8_t(_axis);
// }

// static Antomic::GamepadAxis::Enum translateGamepadAxis(uint8_t _sdl)
// {
//     return Antomic::GamepadAxis::Enum(stranslateGamepadAxis[_sdl]);
// }

namespace Antomic
{

    InputSDL::InputSDL() : mHandler(nullptr)
    {
        std::memset(stranslateKey, 0, sizeof(stranslateKey));
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
    }

    InputSDL::~InputSDL()
    {
    }

    void InputSDL::ProcessEvents()
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
                    mHandler(event);
                }
                break;
                case SDL_WINDOWEVENT_CLOSE:
                {
                    WindowCloseEvent event;
                    mHandler(event);
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
                mHandler(event);
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
                    mHandler(event);
                }
                break;
                case SDL_RELEASED:
                {
                    MouseButtonReleasedEvent event(button);
                    mHandler(event);
                }
                break;
                }
            }
            break;
            case SDL_MOUSEWHEEL:
            {
                const SDL_MouseWheelEvent &mev = currentEvent.wheel;
                MouseScrolledEvent event(mev.x, mev.y);
                mHandler(event);
            }
            break;
            case SDL_KEYDOWN:
            {
                const SDL_KeyboardEvent &kev = currentEvent.key;
                uint8_t modifiers = translateKeyModifiers(kev.keysym.mod);
                Key::Enum key = translateKey(kev.keysym.scancode);

#if ANTOMIC_DEBUG
                ANTOMIC_INFO("SDL scancode {0}, key {1}, name {2}, key name {3}", kev.keysym.scancode, key, SDL_GetScancodeName(kev.keysym.scancode), SDL_GetKeyName(kev.keysym.scancode));
#endif

                /// If you only press (e.g.) 'shift' and nothing else, then key == 'shift', modifier == 0.
                /// Further along, pressing 'shift' + 'ctrl' would be: key == 'shift', modifier == 'ctrl.
                if (0 == key && 0 == modifiers)
                {
                    modifiers = translateKeyModifierPress(kev.keysym.scancode);
                }
                mKeyState.Keys[key] = true;
                KeyPressedEvent event(key, modifiers, kev.repeat);
                mHandler(event);
            }
            break;

            case SDL_KEYUP:
            {
                const SDL_KeyboardEvent &kev = currentEvent.key;
                uint8_t modifiers = translateKeyModifiers(kev.keysym.mod);
                Key::Enum key = translateKey(kev.keysym.scancode);
                KeyReleasedEvent event(key, modifiers);
                mKeyState.Keys[key] = false;
                mHandler(event);
            }
            break;
            default:
                break;
            }
        }
    }
} // namespace Antomic
#endif