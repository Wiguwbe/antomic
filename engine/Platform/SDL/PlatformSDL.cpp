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

#include <SDL2/SDL_syswm.h>
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

namespace Engine
{
    SDL_Window *PlatformSDL::s_SDLWindow = nullptr;

    PlatformSDL::PlatformSDL()
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
            if (currentEvent.type == SDL_WINDOWEVENT)
            {
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