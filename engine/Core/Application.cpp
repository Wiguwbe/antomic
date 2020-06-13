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
#include "Core/Application.h"
#include "Core/Log.h"
#include "Platform/Platform.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

Engine::Application::Application(const char *name, uint32_t width, uint32_t height)
{
    m_Name = std::string(name);
    m_Width = width;
    m_Height = height;
    m_Running = false;
    m_Platform = Platform::Create();;
    m_Platform->SetApplication(this);
}

Engine::Application::~Application()
{
}

bool Engine::Application::Init()
{
    Log::Init();

    if (std::filesystem::exists("settings.json"))
    {
        std::ifstream settingsFile("settings.json");
        nlohmann::json settingsJSON;
        settingsFile >> settingsJSON;

        m_Width = settingsJSON["width"];
        m_Height = settingsJSON["height"];
    }

    if (!m_Platform->Init())
    {
        ENGINE_INFO("Error initializing platform");
        return false;
    }

    if (!m_Platform->CreateWindow(m_Width, m_Height, 0, m_Name))
    {
        ENGINE_INFO("Error creating window: {0}, {1}", m_Width, m_Height);
        return false;
    }

    bgfx::renderFrame();
    bgfx::init();
    bgfx::reset(m_Width, m_Height, BGFX_RESET_VSYNC);

#ifndef NDEBUG
    bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS */);
#endif

    bgfx::setViewRect(0, 0, 0, uint16_t(m_Width), uint16_t(m_Height));
    bgfx::setViewClear(0,
                       BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x000000, 1.0f, 0);
    bgfx::touch(0);

    ENGINE_INFO("Game initialized, window: {0}, {1}", m_Width, m_Height);
    return true;
}

void Engine::Application::Shutdown()
{
    m_Running = false;
}

void Engine::Application::run()
{
    if (m_Running)
        return;

    if (!Init())
        return;

    m_Running = true;

    while (m_Running)
    {
        m_Platform->ProcessWindowEvents();
        this->Update();
    }

    bgfx::shutdown();
    m_Platform->DestroyWindow();
    m_Platform->Shutdown();
}

void Engine::Application::Update()
{
    bgfx::frame();
}

void Engine::Application::ToggleFullscreen(bool value) {
    
}
