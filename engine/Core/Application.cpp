#include "Application.h"
#include "../Platform/Platform.h"
#include "Log.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "nlohmann/json.hpp"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

Engine::Application::Application(const char *name, uint32_t width, uint32_t height, bool fullscreen)
{
    mName = std::string(name);
    mWidth = width;
    mHeight = height;
    mFullscreen = fullscreen;
    mRunning = false;
}

Engine::Application::~Application()
{
}

bool Engine::Application::Init()
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
        Logger.Info("Error initializing platform");
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

    Logger.Info("Game initialized");
    return true;
}

void Engine::Application::Shutdown()
{
    mRunning = false;
}

void Engine::Application::run()
{
    if (mRunning)
        return;

    if (!Init())
        return;

    mRunning = true;

    while (mRunning)
    {
        processEvents(this);
        this->Update();
        bgfx::frame();
    }

    bgfx::shutdown();
    shutdownPlatform();
}