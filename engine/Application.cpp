#include "Application.h"
#include "Platform.h"
#include "Log.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "nlohmann/json.hpp"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

Application::Application(const char *name, uint32_t width, uint32_t height, bool fullscreen)
{
    mName = std::string(name);
    mWidth = width;
    mHeight = height;
    mFullscreen = fullscreen;
    mRunning = false;
}

Application::~Application()
{
}

bool Application::init()
{
    if (std::filesystem::exists("settings.json"))
    {
        std::ifstream settingsFile("settings.json");
        nlohmann::json settingsJSON;
        settingsFile >> settingsJSON;

        this->mWidth = settingsJSON["width"];
        this->mHeight = settingsJSON["height"];
        this->mFullscreen = settingsJSON["fullscreen"];
    }

    if (!initializePlatform(mWidth, mHeight, 0, mName))
    {
        Logger.info("Error initializing platform");
        return false;
    }

    bgfx::renderFrame();
    bgfx::init();
    bgfx::reset(mWidth, mHeight, BGFX_RESET_VSYNC);

#ifndef NDEBUG
    bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS */);
#endif

    bgfx::setViewRect(0, 0, 0, uint16_t(mWidth), uint16_t(mHeight));
    bgfx::setViewClear(0,
                       BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x000000, 1.0f, 0);
    bgfx::touch(0);

    Logger.info("Game initialized");
    return true;
}

void Application::shutdown()
{
    bgfx::shutdown();
    shudownPlatform();
}

void Application::run()
{
    if (mRunning)
        return;

    if (!init())
        return;

    mRunning = true;

    while (mRunning)
    {
        if (!processEvents())
            break;

        if (!this->update())
            break;

        bgfx::frame();
    }

    shutdown();
}