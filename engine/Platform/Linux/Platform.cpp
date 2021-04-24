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
#ifdef ENGINE_PLATFORM_LINUX
#include "Platform/Windows/Platform.h"
#include "Core/Log.h"
#ifdef ENGINE_LINUX_SDL_PLATFORM
#include "Platform/Linux/SDL/Window.h"
#include "Platform/Linux/SDL/Input.h"
#endif

namespace Antomic
{
    LinuxPlatform::LinuxPlatform() 
    {

    }

    LinuxPlatform::~LinuxPlatform()
    {

    }

    Scope<Window> LinuxPlatform::CreateWindow(uint32_t width, uint32_t height, std::string name, RenderAPI api) 
    {
#ifdef ENGINE_SDL_PLATFORM
        return CreateScope<SDLWindow>(width,height,name,api);
#else
        ENGINE_ASSERT(false, "LinuxPlatform: No window support!");
        return nullptr;
#endif            
    }

	Scope<Input> LinuxPlatform::CreateInput()
	{
#ifdef ENGINE_SDL_PLATFORM
        return CreateScope<InputSDL>();
#else
        ENGINE_ASSERT(false, "LinuxPlatform: No input support!");
		return nullptr;
#endif
	}

} // namespace Antomic

#endif