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
#include "Platform/Platform.h"

#ifdef ENGINE_SDL_PLATFORM
#include "Platform/SDL/PlatformSDL.h"
#elif ENGINE_PLATFORM_WINDOWS

#endif

namespace Engine
{
    Scope<Platform> Platform::Create()
    {
#ifdef ENGINE_SDL_PLATFORM
        return CreateScope<PlatformSDL>();
#elif ENGINE_PLATFORM_WINDOWS
        return nullptr;
#else ENGINE_SDL_PLATFORM
        ENGINE_ASSERT(false, "Unknown platform!");
        return nullptr;
#endif
    }
} // namespace Engine
