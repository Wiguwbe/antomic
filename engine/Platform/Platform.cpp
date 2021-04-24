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
#include "Core/Log.h"
#include "Platform/RenderAPI.h"
#ifdef ANTOMIC_PLATFORM_WINDOWS
#include "Platform/Windows/Platform.h"
#elif ANTOMIC_PLATFORM_LINUX
#include "Platform/Linux/Platform.h"
#endif

namespace Antomic
{
    Scope<RenderAPI> Platform::mRenderAPI = nullptr;
    RenderAPIDialect Platform::mRenderAPIDialect = RenderAPIDialect::NONE;

    Scope<Platform> Platform::CreatePlatform(RenderAPIDialect api)
    {
#ifdef ANTOMIC_PLATFORM_WINDOWS
        Scope<Platform> platform = CreateScope<WindowsPlatform>();
#elif ANTOMIC_PLATFORM_LINUX
        Scope<Platform> platform = CreateScope<LinuxPlatform>();
#else
        ANTOMIC_ASSERT(false, "Platform: Platform not supported!");
        return nullptr;
#endif
        ANTOMIC_ASSERT(!mRenderAPI, "Platform: Render API already set!")
        mRenderAPI = RenderAPI::Create(api);
        mRenderAPIDialect = RenderAPIDialect::OPENGL;
        return platform;
    }
} // namespace Antomic
