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
#pragma once
#include "Core/Base.h"
#include "Platform/RenderAPI.h"
#include "Platform/Window.h"
#include "Platform/Input.h"
#include "glm/glm.hpp"

namespace Antomic
{
    class Platform
    {

    public:
        virtual ~Platform() = default;

    protected:
        // Window operations
        virtual Scope<Window> CreateWindow(uint32_t width, uint32_t height, std::string title, RenderAPIDialect api = RenderAPIDialect::OPENGL) = 0;
		virtual Scope<Input> CreateInput() = 0;

        // Time Operations
        virtual uint32_t GetTicks() const = 0;

    public:
        static bool SetupPlatform(uint32_t width, uint32_t height, std::string title, RenderAPIDialect api = RenderAPIDialect::OPENGL);
        
        // Render API
        inline static const Scope<RenderAPI> &GetRenderAPI() { return sRenderAPI; }
        inline static RenderAPIDialect GetRenderAPIDialect() { return sRenderAPIDialect; }

        // Input Operations & Handling
        inline static const Scope<Input> &GetInput() { return sInput; }
        inline static bool IsKeyPressed(Key::Enum key) { return sInput->IsKeyPressed(key); }
		inline bool IsMouseButtonPressed(MouseButton::Enum button) { return sInput->IsMouseButtonPressed(button);}
		inline uint8_t GetKeyModifiers() { return sInput->GetKeyModifiers(); }
		inline glm::vec3 GetMousePosition() { return sInput->GetMousePosition(); }

        // Window Operations & Handling
        inline static const Scope<Window> &GetWindow() { return sWindow; }
        inline static void SwapBuffer() { sWindow->SwapBuffer(); }
        inline static uint32_t GetWindowWidth() { return sWindow->GetWidth(); }
        inline static uint32_t GetWindowHeight() { return sWindow->GetHeight(); }
        inline static const std::string &GetWindowTitle() { return sWindow->GetTitle(); }
        inline static void SetWindowTitle(const std::string& title) { sWindow->SetTitle(title); }
        
        // Events Handling
        inline static void SetEventHandler(const EventHandler& handler)
        {
            sInput->SetEventHandler(handler);
            sWindow->SetEventHandler(handler);
        }
        
        inline static void ProcessEvents() 
        { 
            sInput->ProcessEvents();
            sWindow->ProcessEvents(); 
        }
        
        // Time Operations 
        inline static uint32_t GetCurrentTick() { return sPlatform->GetTicks(); }
    
    private:
        static Scope<Platform> sPlatform;
        static RenderAPIDialect sRenderAPIDialect;
        static Scope<RenderAPI> sRenderAPI;
        static Scope<Window> sWindow;
        static Scope<Input> sInput;
    };

} // namespace Antomic