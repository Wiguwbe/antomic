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
        mWindow = nullptr;
#ifdef ANTOMIC_GL_RENDERER
        mGLContext = nullptr;
#endif
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            ANTOMIC_TRACE("SDLWindow: Error initializing SDL");
            return;
        }

        switch (api)
        {
#ifdef ANTOMIC_GL_RENDERER
        case RenderAPIDialect::OPENGL:

            ANTOMIC_INFO("SDLWindow: Creating window {0},{1} with OpenGL support", width, height);
            mWindow = SDL_CreateWindow(
                title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

            if (!mWindow)
            {
                ANTOMIC_TRACE("SDLWindow: Error creating SDL window: {0},{1}", width, height);
                SDL_Quit();
            }

            mGLContext = SDL_GL_CreateContext(mWindow);
            if (!mGLContext)
            {
                ANTOMIC_TRACE("SDLWindow: Error creating OpenGL context");
                SDL_DestroyWindow(mWindow);
                mWindow = nullptr;
                SDL_Quit();
            }

            if (!gladLoadGL())
            {
                ANTOMIC_TRACE("SDLWindow: Error initializing Glad");
                SDL_GL_DeleteContext(mGLContext);
                SDL_DestroyWindow(mWindow);
                mGLContext = nullptr;
                mWindow = nullptr;
                SDL_Quit();
            }

            ANTOMIC_INFO("SDLWindow: Using OpenGL version {0}", glGetString(GL_VERSION));

            break;
#endif
        default:
            ANTOMIC_INFO("SDLWindow: Creating window {0},{1} with no Render API support", width, height);
            mWindow = SDL_CreateWindow(
                title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            if (!mWindow)
            {
                ANTOMIC_TRACE("SDLWindow: Error creating SDL window: {0},{1}", width, height);
                SDL_Quit();
            }
            break;
        }

        SDL_SysWMinfo wmi;
        SDL_VERSION(&wmi.version);

        if (!SDL_GetWindowWMInfo(mWindow, &wmi))
        {
            ANTOMIC_TRACE("SDLWindow: Error retrieving window information");
#ifdef ANTOMIC_GL_RENDERER
            SDL_GL_DeleteContext(mGLContext);
            mGLContext = nullptr;
#endif
            SDL_DestroyWindow(mWindow);
            mWindow = nullptr;
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
        if (mWindow)
        {
            SDL_DestroyWindow(mWindow);
            SDL_Quit();
        }
    }

    void SDLWindow::SetTitle(const std::string &title)
    {
        if (mWindow)
        {
            SDL_SetWindowTitle(mWindow, title.c_str());
        }
        mTitle = title;
    }

    void SDLWindow::SwapBuffer()
    {
#ifdef ANTOMIC_GL_RENDERER
        SDL_GL_SwapWindow(mWindow);
#endif
    }

    void SDLWindow::ProcessEvents()
    {
        // On SDL everything is handled in the same loop, the main loop is
        // implemented on the InputSDL module
        SDL_GetWindowSize(mWindow, (int *)&mWidth, (int *)&mHeight);
    }

    void SDLWindow::ToggleFullscreen()
    {
    }

    void SDLWindow::SetMouseLock(bool lock)
    {
    }

}
#endif
