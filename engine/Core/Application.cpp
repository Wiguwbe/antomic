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
#include "Platform/Platform.h"
#include "Renderer/Renderer.h"

namespace Engine
{
    Application *Application::s_Instance = nullptr;

    Application::Application(const char *name, uint32_t width, uint32_t height)
    {
        ENGINE_ASSERT(!s_Instance, "Application already running!");
        Log::Init();

        s_Instance = this;
        m_Running = false;
        m_Platform = Platform::Create();
        m_Platform->SetEventHandler(ENGINE_BIND_EVENT_FN(Application::OnEvent));

        auto _width = width;
        auto _height = height;

        if (std::filesystem::exists("settings.json"))
        {
            std::ifstream settingsFile("settings.json");
            nlohmann::json settingsJSON;
            settingsFile >> settingsJSON;

            _width = settingsJSON["width"];
            _height = settingsJSON["height"];
        }

        if (!m_Platform->Init())
        {
            ENGINE_INFO("Error initializing platform");
            exit(1);
        }

        if (!m_Platform->CreateWindow(_width, _height, name))
        {
            ENGINE_INFO("Error creating window: {0}, {1}", _width, _height);
            exit(1);
        }

        Renderer::Init(_width, _height);
    }

    Application::~Application()
    {
    }

    void Application::Run()
    {
        if (m_Running)
            return;

        m_Running = true;

        while (m_Running)
        {
            m_Platform->ProcessWindowEvents();
            this->Update();
            Renderer::RenderFrame();
        }

        Renderer::Shutdown();
        m_Platform->DestroyWindow();
        m_Platform->Shutdown();
    }

    void Application::Update()
    {
    }

    void Application::ToggleFullscreen(bool value)
    {
    }

    void Application::OnEvent(Event &event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(ENGINE_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(ENGINE_BIND_EVENT_FN(Application::OnWindowResize));
    }

    bool Application::OnWindowClose(WindowCloseEvent &event)
    {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent &event)
    {
        Renderer::OnWindowResize(event.GetWidth(), event.GetHeight());
        return true;
    }
} // namespace Engine
