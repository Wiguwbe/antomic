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
#include "Core/Log.h"

#ifdef ENGINE_PLATFORM_LINUX
#include <SDL2/SDL_syswm.h>
#elif ENGINE_PLATFORM_WINDOWS
#include "SDL_syswm.h"
#endif
#ifdef ENGINE_GL_RENDERER
#include <glad/glad.h>
#endif

namespace Antomic
{
    SDL_Window *PlatformSDL::s_SDLWindow = nullptr;
#ifdef ENGINE_GL_RENDERER
    SDL_GLContext PlatformSDL::s_GLContext;
#endif

    PlatformSDL::PlatformSDL() : m_Handler(nullptr)
    {
    }

    PlatformSDL::~PlatformSDL()
    {
    }

    bool PlatformSDL::Init()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            ENGINE_TRACE("PlatformSDL: Error initializing SDL");
            return false;
        }

        return true;
    }

    void PlatformSDL::Shutdown()
    {
        SDL_Quit();
    }

    bool PlatformSDL::SetupWindow(uint32_t width, uint32_t height, std::string name, RenderAPI api)
    {
        m_Width = width;
        m_Height = height;

        switch (api)
        {
        case RenderAPI::OPENGL:
#ifdef ENGINE_GL_RENDERER

            ENGINE_INFO("PlatformSDL: Creating window {0},{1} with OpenGL support", width, height);
            s_SDLWindow = SDL_CreateWindow(
                name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

            if (!s_SDLWindow)
            {
                ENGINE_TRACE("PlatformSDL: Error creating SDL window: {0},{1}", width, height);
                SDL_Quit();
                return false;
            }

            s_GLContext = SDL_GL_CreateContext(s_SDLWindow);
            if (!s_GLContext)
            {
                ENGINE_TRACE("PlatformSDL: Error creating OpenGL context");
                SDL_DestroyWindow(s_SDLWindow);
                SDL_Quit();
                return false;
            }

            if (!gladLoadGL())
            {
                ENGINE_TRACE("PlatformSDL: Error initializing Glad");
                SDL_GL_DeleteContext(s_GLContext);
                SDL_DestroyWindow(s_SDLWindow);
                SDL_Quit();
                return false;
            }

            ENGINE_INFO("PlatformSDL: Using OpenGL version {0}", glGetString(GL_VERSION));

            break;

#else
            ENGINE_ASSERT(false, "Renderer: OpenGL not available!");
            ENGINE_TRACE("PlatformSDL: Error creating SDL window: {0},{1}", width, height);
            SDL_Quit();
            break;
#endif
            break;
        default:
            ENGINE_INFO("PlatformSDL: Creating window {0},{1} with no Render API support", width, height);
            s_SDLWindow = SDL_CreateWindow(
                name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            break;
            if (!s_SDLWindow)
            {
                ENGINE_TRACE("PlatformSDL: Error creating SDL window: {0},{1}", width, height);
                SDL_Quit();
                return false;
            }
        }

        SDL_SysWMinfo wmi;
        SDL_VERSION(&wmi.version);

        if (!SDL_GetWindowWMInfo(s_SDLWindow, &wmi))
        {
            ENGINE_TRACE("PlatformSDL: Error retrieving window information");
#ifdef ENGINE_GL_RENDERER
            SDL_GL_DeleteContext(s_GLContext);
#endif
            SDL_DestroyWindow(s_SDLWindow);
            SDL_Quit();
            return false;
        }

        return true;
    }

    void PlatformSDL::DestroyWindow()
    {
#ifdef ENGINE_GL_RENDERER
        SDL_GL_DeleteContext(s_GLContext);
#endif
        SDL_DestroyWindow(s_SDLWindow);
    }

    void PlatformSDL::UpdateWindow()
    {
#ifdef ENGINE_GL_RENDERER
        SDL_GL_SwapWindow(s_SDLWindow);
#endif
    }

    void PlatformSDL::ProcessWindowEvents()
    {
        // On SDL everything is handled in the same loop, the main loop is
        // implemented on the InputSDL module
    }

    void PlatformSDL::ToggleFullscreen()
    {
    }

    void PlatformSDL::SetMouseLock(bool lock)
    {
    }

} // namespace Antomic

#endif