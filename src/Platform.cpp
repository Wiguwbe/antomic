#include "Platform.h"
#include "Log.h"
#include "Game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

/*
This code is related to the use of SDL, most probably
will be changed in the future 
*/
static SDL_Window* sdlWindow;

bool platform::InitializePlatform(int width, int height, bool fullscreen) 
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        game::Logger.error("Error initializing video");
        return false;
    }

    sdlWindow = SDL_CreateWindow(
        "Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height, SDL_WINDOW_SHOWN);

    if (!sdlWindow)
    {
        game::Logger.error("Error creating window");
        SDL_Quit();
        return false;
    }

    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);

    if (!SDL_GetWindowWMInfo(sdlWindow, &wmi))
    {
        game::Logger.error("Error initializing renderer");
        SDL_DestroyWindow(sdlWindow);
        SDL_Quit();
        return false;
    }

    bgfx::PlatformData pd;
    pd.ndt = wmi.info.x11.display;
    pd.nwh = (void *)(uintptr_t)wmi.info.x11.window;
    bgfx::setPlatformData(pd);
    bgfx::renderFrame();
    bgfx::init();
    bgfx::reset(width, height, BGFX_RESET_VSYNC);

#ifndef NDEBUG
    bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS */);
#endif

    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
    bgfx::setViewClear(0,
                       BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x000000, 1.0f, 0);
    bgfx::touch(0);

    return true;
}

void platform::FinalizePlatform() {
    bgfx::shutdown();
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
}

void platform::RunPlatform(game::Game* game) {
    bool running = True;
    SDL_Event currentEvent;
    while (running)
    {
        while (SDL_PollEvent(&currentEvent) != 0)
        {
            if (currentEvent.type == SDL_QUIT)
            {
                running = False;
            }
            bgfx::frame();
            game->update();
        }
    }
}