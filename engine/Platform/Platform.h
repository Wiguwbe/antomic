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
#include "Core/Log.h"
#include "Profiling/Instrumentor.h"
#include "Core/Application.h"

namespace Engine
{
    class Platform
    {

    public:
        virtual ~Platform() = default;

    public:
        static Scope<Platform> Create();

        virtual bool Init() = 0;
        virtual void Shutdown() = 0;
        virtual void ToggleFullscreen() = 0;
        virtual void SetMouseLock(bool lock) = 0;
        virtual bool SetupWindow(uint32_t width, uint32_t height, std::string name) = 0;
        virtual void DestroyWindow() = 0;
        virtual void ProcessWindowEvents() = 0;
        virtual void SetEventHandler(const EventHandler& handler) = 0;

    };

} // namespace Engine