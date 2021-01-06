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
#ifdef ENGINE_PLATFORM_WINDOWS

#include "Platform/Windows/PlatformWindows.h"
#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
//#include "bx/bx.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"



namespace Engine
{

    PlatformWindows::PlatformWindows()
    {
    }

    PlatformWindows::~PlatformWindows()
    {
    }

    bool PlatformWindows::Init()
    {
        return false;
    }

    void PlatformWindows::Shutdown()
    {
    }

    bool PlatformWindows::CreateWindow(uint32_t width, uint32_t height, std::string name)
    {
        return false;
    }

    void PlatformWindows::DestroyWindow()
    {
    }

    void PlatformWindows::ProcessWindowEvents()
    {
    }

    void PlatformWindows::ToggleFullscreen()
    {
    }

    void PlatformWindows::SetMouseLock(bool lock)
    {
    }

} // namespace Engine

#endif