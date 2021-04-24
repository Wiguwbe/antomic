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
    Application *Application::sInstance = nullptr;

    Application::Application(const std::string &title, uint32_t width, uint32_t height, RenderAPI api)
    {
        ENGINE_ASSERT(!sInstance, "Application: Application already running!");
        Log::Init();

        sInstance = this;
        mRunning = false;
        mPlatform = Platform::Create();
        ENGINE_ASSERT(mPlatform, "Application: Platform not initialized!");

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

        mWindow = mPlatform->CreateWindow(_width, _height, title, _api);
        ENGINE_ASSERT(mWindow, "Application: Window not created!");
        mWindow->SetEventHandler(ENGINE_BIND_EVENT_FN(Application::OnEvent));
        if (!mWindow->IsValid())
        {
            ENGINE_INFO("Error creating window: {0}, {1}", _width, _height);
            exit(1);
        }

        mRenderer = CreateScope<Renderer>(_api);
        if (!mRenderer)
        {
            ENGINE_INFO("Error creating rendering API: {0}", RenderAPIToStr(_api));
            exit(1);
        }

        mInput = mPlatform->CreateInput();
        mInput->SetEventHandler(ENGINE_BIND_EVENT_FN(Application::OnEvent));

        if (!mInput->SetupInput())
        {
            ENGINE_INFO("Error initializng input:");
            exit(1);
        }

        mWidth = _width;
        mHeight = _height;
    }

    Application::~Application()
    {
        this->mInput = nullptr;
    }

    void Application::Run()
    {
        if (mRunning)
            return;

        mRunning = true;

        this->mRenderer->Init(mWidth, mHeight);

        mLastRenderTime = mPlatform->GetTicks();
        while (mRunning)
        {
            auto currentTime = mPlatform->GetTicks();
            uint32_t t = currentTime - mLastRenderTime;
            mLastRenderTime = currentTime;
            
            mInput->ProcessEvents();
            mWindow->ProcessEvents();
            
            
            mStack.Update(t);
            this->Update(t);
            
            mRenderer->BeginScene();
            mRenderer->EndScene();
            this->Render();
            mStack.Render();            
            this->mRenderer->Flush();
            
            mWindow->Update();
        }

        // We clean the layer stack since some
        // layers might need to clean some resources
        while (!mStack.Empty())
        {
            mStack.PopFront();
        }

        this->mRenderer->Shutdown();
    }

    void Application::ToggleFullscreen(bool value)
    {
    }

    void Application::OnEvent(Event &event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(ENGINE_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(ENGINE_BIND_EVENT_FN(Application::OnWindowResize));
        mStack.OnEvent(event);
    }

    bool Application::OnWindowClose(WindowCloseEvent &event)
    {
        mRunning = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent &event)
    {
        this->mRenderer->OnWindowResize(event.GetWidth(), event.GetHeight());
        mWidth = event.GetWidth();
        mHeight = event.GetHeight();
        return true;
    }
} // namespace Antomic
