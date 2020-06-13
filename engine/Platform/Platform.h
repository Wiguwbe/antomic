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
#include "Core/Window.h"
#include "Core/Application.h"

#define CurrentPlatform Platform::GetInstance()

namespace Engine
{

    class Platform : public Singleton<Platform>
    {

    public:
        Platform();

    public:
        WindowHandle CreateWindow(int32_t _x, int32_t _y, uint32_t _width, uint32_t _height, uint32_t _flags = 0, const char *_title = "");
        void DestroyWindow(WindowHandle _handle);
        void SetWindowPos(WindowHandle _handle, int32_t _x, int32_t _y);
        void SetWindowSize(WindowHandle _handle, uint32_t _width, uint32_t _height);
        void SetWindowTitle(WindowHandle _handle, const char *_title);
        void SetWindowFlags(WindowHandle _handle, uint32_t _flags, bool _enabled);
        void ToggleFullscreen(WindowHandle _handle);
        void SetMouseLock(WindowHandle _handle, bool _lock);
        void ProcessWindowEvents(WindowState& _state, uint32_t& _debug, uint32_t& _reset);

        bool Init(uint32_t width, uint32_t height, uint32_t flags, std::string name);
        void Shutdown();
        void ProcessEvents(Application *app);
    };

} // namespace Engine