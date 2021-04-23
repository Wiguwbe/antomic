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
#ifdef ENGINE_SDL_PLATFORM
#include "Platform/Platform.h"
#ifdef ENGINE_PLATFORM_LINUX
#include <SDL2/SDL.h>
#elif ENGINE_PLATFORM_WINDOWS
#include "SDL.h"
#endif

namespace Antomic
{

    class PlatformSDL : public Platform
    {
    public:
        PlatformSDL();
        virtual ~PlatformSDL();

        virtual bool Init() override;
        virtual void Shutdown()override;
        virtual void ToggleFullscreen() override;
        virtual void SetMouseLock(bool lock) override;
        virtual bool SetupWindow(uint32_t width, uint32_t height, std::string name, RenderPlatform platform = RenderPlatform::OPENGL) override;
        virtual void DestroyWindow() override;
        virtual void UpdateWindow() override;
        virtual void ProcessWindowEvents() override;
        virtual void SetEventHandler(const EventHandler &handler) override { m_Handler = handler; } 

    private:
        static SDL_Window *s_SDLWindow;
#ifdef ENGINE_GL_RENDERER
        static SDL_GLContext s_GLContext;
#endif
        std::string m_Name;
        uint32_t m_Width;
        uint32_t m_Height;
        EventHandler m_Handler;
    };
} // namespace Antomic
#endif