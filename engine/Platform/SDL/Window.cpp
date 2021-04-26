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
#include "Platform/SDL/Window.h"
#include "Platform/RenderAPI.h"
#include "Core/Log.h"
#include "SDL2/SDL_syswm.h"
#ifdef ANTOMIC_GL_RENDERER
#include <glad/glad.h>
#endif

namespace Antomic
{
    SDLWindow::SDLWindow(uint32_t width, uint32_t height, std::string title, RenderAPIDialect api)
    {
        mSDLWindow = nullptr;
#ifdef ANTOMIC_GL_RENDERER
        mGLContext = nullptr;
#endif
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            ANTOMIC_TRACE("SDLWindow: Error initializing SDL");
        }

        switch (api)
        {
        case RenderAPIDialect::OPENGL:
#ifdef ANTOMIC_GL_RENDERER

            ANTOMIC_INFO("SDLWindow: Creating window {0},{1} with OpenGL support", width, height);
            mSDLWindow = SDL_CreateWindow(
                title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

            if (!mSDLWindow)
            {
                ANTOMIC_TRACE("SDLWindow: Error creating SDL window: {0},{1}", width, height);
                SDL_Quit();
            }

            mGLContext = SDL_GL_CreateContext(mSDLWindow);
            if (!mGLContext)
            {
                ANTOMIC_TRACE("SDLWindow: Error creating OpenGL context");
                SDL_DestroyWindow(mSDLWindow);
                mSDLWindow = nullptr;
                SDL_Quit();
            }

            if (!gladLoadGL())
            {
                ANTOMIC_TRACE("SDLWindow: Error initializing Glad");
                SDL_GL_DeleteContext(mGLContext);
                SDL_DestroyWindow(mSDLWindow);
                mGLContext = nullptr;
                mSDLWindow = nullptr;
                SDL_Quit();
            }

            ANTOMIC_INFO("SDLWindow: Using OpenGL version {0}", glGetString(GL_VERSION));

            break;

#else
            ANTOMIC_ASSERT(false, "Renderer: OpenGL not available!");
            ANTOMIC_TRACE("SDLWindow: Error creating SDL window: {0},{1}", width, height);
            SDL_Quit();
            break;
#endif
            break;
        default:
            ANTOMIC_INFO("SDLWindow: Creating window {0},{1} with no Render API support", width, height);
            mSDLWindow = SDL_CreateWindow(
                title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            break;
            if (!mSDLWindow)
            {
                ANTOMIC_TRACE("SDLWindow: Error creating SDL window: {0},{1}", width, height);
                SDL_Quit();
            }
        }

        SDL_SysWMinfo wmi;
        SDL_VERSION(&wmi.version);

        if (!SDL_GetWindowWMInfo(mSDLWindow, &wmi))
        {
            ANTOMIC_TRACE("SDLWindow: Error retrieving window information");
#ifdef ANTOMIC_GL_RENDERER
            SDL_GL_DeleteContext(mGLContext);
            mGLContext = nullptr;
#endif
            SDL_DestroyWindow(mSDLWindow);
            mSDLWindow = nullptr;
            SDL_Quit();
        }

        mWidth = width;
        mHeight = height;
        mTitle = title;
    }

    SDLWindow::~SDLWindow()
    {
#ifdef ANTOMIC_GL_RENDERER
        if (mGLContext)
        {
            SDL_GL_DeleteContext(mGLContext);
        }
#endif
        if (mSDLWindow)
        {
            SDL_DestroyWindow(mSDLWindow);
            SDL_Quit();
        }
    }

    void SDLWindow::SetTitle(const std::string &title)
    {
        if (mSDLWindow)
        {
            SDL_SetWindowTitle(mSDLWindow, title.c_str());
        }
        mTitle = title;
    }

    void SDLWindow::SwapChain()
    {
#ifdef ANTOMIC_GL_RENDERER
        SDL_GL_SwapWindow(mSDLWindow);
#endif
    }

    void SDLWindow::ProcessEvents()
    {
        // On SDL everything is handled in the same loop, the main loop is
        // implemented on the InputSDL module
        SDL_GetWindowSize(mSDLWindow, (int *)&mWidth, (int *)&mHeight);
    }

    void SDLWindow::ToggleFullscreen()
    {
    }

    void SDLWindow::SetMouseLock(bool lock)
    {
    }

}
#endif
