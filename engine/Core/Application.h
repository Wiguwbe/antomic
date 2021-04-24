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
#pragma once
#include "Core/Base.h"
#include "Platform/Platform.h"
#include "Core/LayerStack.h"
#include "Platform/Input.h"
#include "Renderer/Renderer.h"

namespace Antomic
{
    class Application
    {

    public:
        Application() : Application("Application", 640, 480){};
        Application(const std::string &title) : Application(title, 640, 480){};
        Application(const std::string &title, uint32_t width, uint32_t height, RenderAPI api = RenderAPI::OPENGL);

        virtual ~Application();

        void OnEvent(Event &event);
        void ToggleFullscreen(bool value);
        void Run();
        bool OnWindowClose(WindowCloseEvent &event);
        bool OnWindowResize(WindowResizeEvent &event);
        inline uint32_t CurrentWidth() const { return mWidth; }
        inline uint32_t CurrentHeight() const { return mHeight; }

        virtual void Update(const uint32_t &time) {};
        virtual void Render() {};

        static Application &Current() { return *sInstance; }

    protected:
        LayerStack mStack;
        uint32_t mWidth, mHeight;

    private:
        static Application *sInstance;
        bool mRunning;
        Scope<Platform> mPlatform;
        Scope<Window> mWindow;
        Scope<Input> mInput;
        Scope<Renderer> mRenderer;
        uint32_t mLastRenderTime;
    };
} // namespace Antomic
