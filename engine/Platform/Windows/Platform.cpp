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
#ifdef ANTOMIC_PLATFORM_WINDOWS
#include "Platform/Windows/Platform.h"
#include "Core/Log.h"
#ifdef ANTOMIC_WINDOWS_SDL_PLATFORM
#include "Platform/Windows/SDL/Window.h"
#include "Platform/Windows/SDL/Input.h"
#include "Platform/Windows/SDL/Utils.h"
#endif

namespace Antomic
{
    WindowsPlatform::WindowsPlatform() 
    {
#ifdef ANTOMIC_CHRONO_SUPPORT
        mPlatformStart = std::chrono::high_resolution_clock::now();
#endif
    }

    WindowsPlatform::~WindowsPlatform()
    {

    }

    Scope<Window> WindowsPlatform::CreateWindow(uint32_t width, uint32_t height, std::string name, RenderAPIDialect api) 
    {
#ifdef ANTOMIC_SDL_PLATFORM
        return CreateScope<SDLWindow>(width,height,name,api);
#else
        ANTOMIC_ASSERT(false, "WindowsPlatform: No window support!");
        return nullptr;
#endif            
    }

	Scope<Input> WindowsPlatform::CreateInput()
	{
#ifdef ANTOMIC_SDL_PLATFORM
        return CreateScope<InputSDL>();
#else
        ANTOMIC_ASSERT(false, "WindowsPlatform: No input support!");
		return nullptr;
#endif
	}

    uint32_t WindowsPlatform::GetTicks() const 
    {
#ifdef ANTOMIC_SDL_PLATFORM
        return GetCurrentTime();
#elif ANTOMIC_CHRONO_SUPPORT
        auto now = std::chrono::high_resolution_clock::now();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - mPlatformStart);
        return milliseconds.count();
#else
    ANTOMIC_ASSERT(false, "LinuxPlatform: No time tick support!");
    return nullptr;
#endif
    }
} // namespace Antomic

#endif