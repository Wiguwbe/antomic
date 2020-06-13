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
#include "Platform/Platform.h"
#include <SDL2/SDL.h>

namespace Engine
{

    class PlatformSDL : public Platform
    {
    public:
        PlatformSDL();
        virtual ~PlatformSDL();

        virtual bool Init();
        virtual void Shutdown();
        virtual void ToggleFullscreen();
        virtual void SetMouseLock(bool lock);
        virtual bool CreateWindow(uint32_t width, uint32_t height, std::string name);
        virtual void DestroyWindow();
        virtual void ProcessWindowEvents();
        virtual void SetEventHandler(const EventHandler &handler) { m_Handler = handler; }

    private:
        static SDL_Window *s_SDLWindow;
        std::string m_Name;
        uint32_t m_Width;
        uint32_t m_Height;
        EventHandler m_Handler;
    };
} // namespace Engine