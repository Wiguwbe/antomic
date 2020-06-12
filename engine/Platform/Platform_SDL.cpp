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
#include "Core/Log.h"
#include "Core/Application.h"
#include "Platform/Platform.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

static SDL_Window *sdlWindow;

Engine::Platform::Platform()
{
}

bool Engine::Platform::Init(uint32_t width, uint32_t height, uint32_t flags, std::string name)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        Engine::Logger.Error("Error initializing video");
        return false;
    }

    sdlWindow = SDL_CreateWindow(
        name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height, SDL_WINDOW_SHOWN);

    if (!sdlWindow)
    {
        Engine::Logger.Error("Error creating window");
        SDL_Quit();
        return false;
    }

    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);

    if (!SDL_GetWindowWMInfo(sdlWindow, &wmi))
    {
        Engine::Logger.Error("Error initializing renderer");
        SDL_DestroyWindow(sdlWindow);
        SDL_Quit();
        return false;
    }

    bgfx::PlatformData pd;
    pd.ndt = wmi.info.x11.display;
    pd.nwh = (void *)(uintptr_t)wmi.info.x11.window;
    bgfx::setPlatformData(pd);

    return true;
}

void Engine::Platform::Shutdown()
{
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
}

void Engine::Platform::ProcessEvents(Application *app)
{
    SDL_Event currentEvent;

    while (SDL_PollEvent(&currentEvent))
    {
        if (currentEvent.type == SDL_QUIT)
            app->Shutdown();
    }
}