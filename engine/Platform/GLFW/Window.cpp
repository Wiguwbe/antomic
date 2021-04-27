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
#ifdef ANTOMIC_GLFW_PLATFORM
#include "Platform/GLFW/Window.h"
#include "Events/WindowEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Core/Log.h"

namespace Antomic
{

    static uint8_t stranslateKey[256];

    static void initTranslateKey(uint16_t _glfw, Antomic::Key::Enum _key)
    {
        stranslateKey[_glfw & 0xff] = (uint8_t)_key;
    }

    static Antomic::Key::Enum translateKey(int _glfw)
    {
        return (Antomic::Key::Enum)stranslateKey[_glfw & 0xff];
    }

    GLFWWindow::GLFWWindow(uint32_t width, uint32_t height, std::string title, RenderAPIDialect api)
    {
        mWindow = nullptr;
        

        if (!glfwInit())
        {
            ANTOMIC_TRACE("GLFWWindow: Error initializing GLFW");
            return;
        }

        ANTOMIC_INFO("GLFWWindow: Creating window {0},{1} with OpenGL support", width, height);
        mWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (!mWindow)
        {
            ANTOMIC_TRACE("GLFWWindow: Error creating SDL window: {0},{1}", width, height);
            return;
        }

        if (!gladLoadGL())
        {
            ANTOMIC_TRACE("GLFWWindow: Error initializing Glad");
            glfwDestroyWindow(mWindow);
            mWindow = nullptr;
        }

        ANTOMIC_INFO("GLFWWindow: Using OpenGL version {0}", glGetString(GL_VERSION));

        std::memset(stranslateKey, 0, sizeof(stranslateKey));
        initTranslateKey(GLFW_KEY_ESCAPE, Key::Esc);
        initTranslateKey(GLFW_KEY_ENTER, Key::Return);
        initTranslateKey(GLFW_KEY_TAB, Key::Tab);
        initTranslateKey(GLFW_KEY_BACKSPACE, Key::Backspace);
        initTranslateKey(GLFW_KEY_SPACE, Key::Space);
        initTranslateKey(GLFW_KEY_UP, Key::Up);
        initTranslateKey(GLFW_KEY_DOWN, Key::Down);
        initTranslateKey(GLFW_KEY_LEFT, Key::Left);
        initTranslateKey(GLFW_KEY_RIGHT, Key::Right);
        initTranslateKey(GLFW_KEY_PAGE_UP, Key::PageUp);
        initTranslateKey(GLFW_KEY_PAGE_DOWN, Key::PageDown);
        initTranslateKey(GLFW_KEY_HOME, Key::Home);
        initTranslateKey(GLFW_KEY_END, Key::End);
        initTranslateKey(GLFW_KEY_PRINT_SCREEN, Key::Print);
        initTranslateKey(GLFW_KEY_KP_ADD, Key::Plus);
        initTranslateKey(GLFW_KEY_EQUAL, Key::Plus);
        initTranslateKey(GLFW_KEY_KP_SUBTRACT, Key::Minus);
        initTranslateKey(GLFW_KEY_MINUS, Key::Minus);
        initTranslateKey(GLFW_KEY_GRAVE_ACCENT, Key::Tilde);
        initTranslateKey(GLFW_KEY_KP_DECIMAL, Key::Comma);
        initTranslateKey(GLFW_KEY_COMMA, Key::Comma);
        initTranslateKey(GLFW_KEY_PERIOD, Key::Period);
        initTranslateKey(GLFW_KEY_SLASH, Key::Slash);
        initTranslateKey(GLFW_KEY_F1, Key::F1);
        initTranslateKey(GLFW_KEY_F2, Key::F2);
        initTranslateKey(GLFW_KEY_F3, Key::F3);
        initTranslateKey(GLFW_KEY_F4, Key::F4);
        initTranslateKey(GLFW_KEY_F5, Key::F5);
        initTranslateKey(GLFW_KEY_F6, Key::F6);
        initTranslateKey(GLFW_KEY_F7, Key::F7);
        initTranslateKey(GLFW_KEY_F8, Key::F8);
        initTranslateKey(GLFW_KEY_F9, Key::F9);
        initTranslateKey(GLFW_KEY_F10, Key::F10);
        initTranslateKey(GLFW_KEY_F11, Key::F11);
        initTranslateKey(GLFW_KEY_F12, Key::F12);
        initTranslateKey(GLFW_KEY_KP_0, Key::NumPad0);
        initTranslateKey(GLFW_KEY_KP_1, Key::NumPad1);
        initTranslateKey(GLFW_KEY_KP_2, Key::NumPad2);
        initTranslateKey(GLFW_KEY_KP_3, Key::NumPad3);
        initTranslateKey(GLFW_KEY_KP_4, Key::NumPad4);
        initTranslateKey(GLFW_KEY_KP_5, Key::NumPad5);
        initTranslateKey(GLFW_KEY_KP_6, Key::NumPad6);
        initTranslateKey(GLFW_KEY_KP_7, Key::NumPad7);
        initTranslateKey(GLFW_KEY_KP_8, Key::NumPad8);
        initTranslateKey(GLFW_KEY_KP_9, Key::NumPad9);
        initTranslateKey(GLFW_KEY_KP_ENTER, Key::NumPadEnter);
        initTranslateKey(GLFW_KEY_0, Key::Key0);
        initTranslateKey(GLFW_KEY_1, Key::Key1);
        initTranslateKey(GLFW_KEY_2, Key::Key2);
        initTranslateKey(GLFW_KEY_3, Key::Key3);
        initTranslateKey(GLFW_KEY_4, Key::Key4);
        initTranslateKey(GLFW_KEY_5, Key::Key5);
        initTranslateKey(GLFW_KEY_6, Key::Key6);
        initTranslateKey(GLFW_KEY_7, Key::Key7);
        initTranslateKey(GLFW_KEY_8, Key::Key8);
        initTranslateKey(GLFW_KEY_9, Key::Key9);
        initTranslateKey(GLFW_KEY_A, Key::KeyA);
        initTranslateKey(GLFW_KEY_B, Key::KeyB);
        initTranslateKey(GLFW_KEY_C, Key::KeyC);
        initTranslateKey(GLFW_KEY_D, Key::KeyD);
        initTranslateKey(GLFW_KEY_E, Key::KeyE);
        initTranslateKey(GLFW_KEY_F, Key::KeyF);
        initTranslateKey(GLFW_KEY_G, Key::KeyG);
        initTranslateKey(GLFW_KEY_H, Key::KeyH);
        initTranslateKey(GLFW_KEY_I, Key::KeyI);
        initTranslateKey(GLFW_KEY_J, Key::KeyJ);
        initTranslateKey(GLFW_KEY_K, Key::KeyK);
        initTranslateKey(GLFW_KEY_L, Key::KeyL);
        initTranslateKey(GLFW_KEY_M, Key::KeyM);
        initTranslateKey(GLFW_KEY_N, Key::KeyN);
        initTranslateKey(GLFW_KEY_O, Key::KeyO);
        initTranslateKey(GLFW_KEY_P, Key::KeyP);
        initTranslateKey(GLFW_KEY_Q, Key::KeyQ);
        initTranslateKey(GLFW_KEY_R, Key::KeyR);
        initTranslateKey(GLFW_KEY_S, Key::KeyS);
        initTranslateKey(GLFW_KEY_T, Key::KeyT);
        initTranslateKey(GLFW_KEY_U, Key::KeyU);
        initTranslateKey(GLFW_KEY_V, Key::KeyV);
        initTranslateKey(GLFW_KEY_W, Key::KeyW);
        initTranslateKey(GLFW_KEY_X, Key::KeyX);
        initTranslateKey(GLFW_KEY_Y, Key::KeyY);
        initTranslateKey(GLFW_KEY_Z, Key::KeyZ);

        glfwMakeContextCurrent(mWindow);
        glfwSetWindowUserPointer(mWindow, &mData);

        // Set GLFW callbacks
        glfwSetWindowSizeCallback(mWindow, [](GLFWwindow *window, int width, int height) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            WindowResizeEvent event(width, height);
            data.Handler(event);
        });

        glfwSetWindowCloseCallback(mWindow, [](GLFWwindow *window) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            data.Handler(event);
        });

        glfwSetKeyCallback(mWindow, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

            switch (action)
            {
            case GLFW_PRESS:
            {
                KeyPressedEvent event(translateKey(key), 0, 0);
                data.Handler(event);
                break;
            }
            case GLFW_RELEASE:
            {
                KeyReleasedEvent event(translateKey(key), 0);
                data.Handler(event);
                break;
            }
            case GLFW_REPEAT:
            {
                KeyPressedEvent event(translateKey(key), 0, 1);
                data.Handler(event);
                break;
            }
            }
        });

        glfwSetCharCallback(mWindow, [](GLFWwindow *window, unsigned int keycode) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

            KeyTypedEvent event(translateKey(keycode), 0);
            data.Handler(event);
        });

        glfwSetMouseButtonCallback(mWindow, [](GLFWwindow *window, int button, int action, int mods) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

            MouseButton::Enum aButton;

            switch (button)
            {
            default:
            case GLFW_MOUSE_BUTTON_LEFT:
                aButton = MouseButton::Left;
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                aButton = MouseButton::Middle;
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                aButton = MouseButton::Right;
                break;
            }

            switch (action)
            {
            case GLFW_PRESS:
            {
                MouseButtonPressedEvent event(aButton);
                data.Handler(event);
                break;
            }
            case GLFW_RELEASE:
            {
                MouseButtonReleasedEvent event(aButton);
                data.Handler(event);
                break;
            }
            }
        });

        glfwSetScrollCallback(mWindow, [](GLFWwindow *window, double xOffset, double yOffset) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.Handler(event);
        });

        glfwSetCursorPosCallback(mWindow, [](GLFWwindow *window, double xPos, double yPos) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

            MouseMovedEvent event((float)xPos, (float)yPos);
            data.Handler(event);
        });

        mData.Width = width;
        mData.Height = height;
        mData.Title = title;
    }

    GLFWWindow::~GLFWWindow()
    {
        if (mWindow)
        {
            glfwDestroyWindow(mWindow);
            mWindow = nullptr;
        }
    }

    void GLFWWindow::SetTitle(const std::string &title)
    {
        if (mWindow)
        {
            glfwSetWindowTitle(mWindow, title.c_str());
        }
        mData.Title = title;
    }

    void GLFWWindow::SwapBuffer()
    {
        glfwSwapBuffers(mWindow);
    }

    void GLFWWindow::ProcessEvents()
    {
        // On SDL everything is handled in the same loop, the main loop is
        // implemented on the InputSDL module
        glfwPollEvents();
    }

    void GLFWWindow::ToggleFullscreen()
    {
    }

    void GLFWWindow::SetMouseLock(bool lock)
    {
    }
}
#endif