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
#include "Renderer/Renderer.h"
#include "Events/ApplicationEvent.h"
#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

namespace Antomic
{
    Application *Application::s_Instance = nullptr;

    Application::Application(const char *name, uint32_t width, uint32_t height, RenderPlatform platform)
    {
        ENGINE_ASSERT(!s_Instance, "Application already running!");
        Log::Init();

        s_Instance = this;
        m_Running = false;
        m_Platform = Platform::Create();
        m_Input = Input::Create();
        m_Renderer = CreateScope<Renderer>(platform);
        m_Platform->SetEventHandler(ENGINE_BIND_EVENT_FN(Application::OnEvent));
        m_Input->SetEventHandler(ENGINE_BIND_EVENT_FN(Application::OnEvent));

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

        if (!m_Platform->SetupWindow(_width, _height, name, platform))
        {
            ENGINE_INFO("Error creating window: {0}, {1}", _width, _height);
            exit(1);
        }

        if (!m_Input->SetupInput())
        {
            ENGINE_INFO("Error initializng input:");
            m_Platform->Shutdown();
            exit(1);
        }

        m_Width = _width;
        m_Height = _height;
    }

    Application::~Application()
    {
        this->m_Input = nullptr;
    }

    void Application::Run()
    {
        if (m_Running)
            return;

        m_Running = true;

        this->BeforeRendererInit();
        this->m_Renderer->Init(m_Width, m_Height);
        this->AfterRendererInit();

        while (m_Running)
        {
            m_Input->ProcessInputEvents();
            m_Platform->ProcessWindowEvents();
            this->Update();
            m_Renderer->BeginScene();
            this->Render();
            m_Renderer->EndScene();
            this->m_Renderer->Flush();
            m_Platform->UpdateWindow();
        }

        // We clean the layer stack since some
        // layers might need to clean some resources
        while (!m_Stack.Empty())
        {
            m_Stack.PopFront();
        }

        this->BeforeRendererShutdown();
        this->m_Renderer->Shutdown();
        this->AfterRendererShutdown();
        m_Platform->DestroyWindow();
        m_Platform->Shutdown();
    }

    void Application::Render()
    {
        m_Stack.Render();
    }

    void Application::Update()
    {
        m_Stack.Update();
    }

    void Application::ToggleFullscreen(bool value)
    {
    }

    void Application::OnEvent(Event &event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(ENGINE_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(ENGINE_BIND_EVENT_FN(Application::OnWindowResize));
        m_Stack.OnEvent(event);
    }

    bool Application::OnWindowClose(WindowCloseEvent &event)
    {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent &event)
    {
        this->m_Renderer->OnWindowResize(event.GetWidth(), event.GetHeight());
        m_Width = event.GetWidth();
        m_Height = event.GetHeight();
        return true;
    }
} // namespace Antomic
