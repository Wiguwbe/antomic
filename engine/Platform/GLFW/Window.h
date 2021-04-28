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
#pragma once
#include "Platform/Window.h"
#include "Platform/RenderAPI.h"
#ifdef ANTOMIC_GL_RENDERER
#include "glad/glad.h"
#endif
#include "GLFW/glfw3.h"

namespace Antomic
{
    class GLFWWindow : public Window
    {
    public:
        GLFWWindow(uint32_t width, uint32_t height, std::string title, RenderAPIDialect api);
        virtual ~GLFWWindow() override;

    public:
        virtual uint32_t GetWidth() const override { return mData.Width; };
        virtual uint32_t GetHeight() const override { return mData.Height; };
        virtual const std::string &GetTitle() const override { return mData.Title; };
        virtual void SetTitle(const std::string &title) override;
        virtual bool IsValid() const override { return mWindow != nullptr; };
        virtual void SetEventHandler(const EventHandler &handler) override { mData.Handler = handler; } 
        virtual void SwapBuffer() override;
        virtual void ProcessEvents() override;
        virtual void ToggleFullscreen() override;
        virtual void SetMouseLock(bool lock) override;
        virtual void* GetNativeWindow() const override { return mWindow; }
        virtual void Close() override;

    private:
        GLFWwindow *mWindow;  
        WindowData mData;
    };
} // namspace Antomic
#endif