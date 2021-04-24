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

namespace Antomic
{
    class Window
    {
    public:
        virtual ~Window() = default;

    public:
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual const std::string &GetTitle() const = 0;
        virtual void SetTitle(const std::string &title) = 0;
        virtual bool IsValid() const = 0;
        virtual void SetEventHandler(const EventHandler &handler) = 0;
        virtual void Update() = 0;
        virtual void ProcessEvents() = 0;
        virtual void ToggleFullscreen() = 0;
        virtual void SetMouseLock(bool lock) = 0;
    };

} // namespace Antomic