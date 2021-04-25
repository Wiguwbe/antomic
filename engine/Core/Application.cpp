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
#include "Events/ApplicationEvent.h"
#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

namespace Antomic
{
    Application *Application::sInstance = nullptr;

    Application::Application(const std::string &title, uint32_t width, uint32_t height, RenderAPIDialect api)
    {
        ANTOMIC_ASSERT(!sInstance, "Application: Application already running!");
        Log::Init();

        sInstance = this;
        mRunning = false;

        auto _width = width;
        auto _height = height;
        auto _api = api;

        if (std::filesystem::exists("settings.json"))
        {
            std::ifstream settingsFile("settings.json");
            nlohmann::json settingsJSON;
            settingsFile >> settingsJSON;

            _width = settingsJSON["width"];
            _height = settingsJSON["height"];
            std::string api_str = settingsJSON["api"];
            _api = RenderAPIFromStr(api_str);
        }

        if (!Platform::SetupPlatform(_width, _height, title, _api))
        {
            ANTOMIC_INFO("Error creating initializing platform");
            exit(1);
        }
        
        Platform::SetEventHandler(ANTOMIC_BIND_EVENT_FN(Application::OnEvent));
    }

    Application::~Application()
    {
    }

    void Application::Run()
    {
        if (mRunning)
            return;

        mRunning = true;

        mLastRenderTime = Platform::GetCurrentTick();
        while (mRunning)
        {
            auto currentTime = Platform::GetCurrentTick();
            uint32_t t = currentTime - mLastRenderTime;
            mLastRenderTime = currentTime;

            Platform::ProcessEvents();

            mStack.Update(t);
            this->Update(t);

            this->Render();
            mStack.Render();

            Platform::UpdateWindow();
        }

        // We clean the layer stack since some
        // layers might need to clean some resources
        while (!mStack.Empty())
        {
            mStack.PopFront();
        }
    }

    void Application::ToggleFullscreen(bool value)
    {
    }

    void Application::OnEvent(Event &event)
    {
        EventDispatcher dispatcher(event);

        // Dispatch Windows Events
        dispatcher.Dispatch<WindowCloseEvent>(ANTOMIC_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(ANTOMIC_BIND_EVENT_FN(Application::OnWindowResize));

        // Dispatch Events to the stack first
        mStack.OnEvent(event);

        // Dispatch key events to be handled by the application
        dispatcher.Dispatch<KeyPressedEvent>(ANTOMIC_BIND_EVENT_FN(Application::OnKeyPressed));
        dispatcher.Dispatch<KeyReleasedEvent>(ANTOMIC_BIND_EVENT_FN(Application::OnKeyReleased));

        // Dispatch mouse events to be handled by the application
        dispatcher.Dispatch<MouseMovedEvent>(ANTOMIC_BIND_EVENT_FN(Application::OnMouseMoved));
        dispatcher.Dispatch<MouseScrolledEvent>(ANTOMIC_BIND_EVENT_FN(Application::OnMouseScrolled));
        dispatcher.Dispatch<MouseButtonPressedEvent>(ANTOMIC_BIND_EVENT_FN(Application::OnMouseButtonPressed));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(ANTOMIC_BIND_EVENT_FN(Application::OnMouseButtonReleased));
    }

    bool Application::OnWindowClose(WindowCloseEvent &event)
    {
        mRunning = false;
        return true;
    }

} // namespace Antomic
