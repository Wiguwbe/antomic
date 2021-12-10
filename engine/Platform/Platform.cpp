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
#	include "Platform/Windows/Platform.h"
#elif ANTOMIC_PLATFORM_LINUX
#	include "Platform/Linux/Platform.h"
#endif

namespace Antomic
{
	Scope<RenderAPI> Platform::sRenderAPI = nullptr;
	RenderAPIDialect Platform::sRenderAPIDialect = RenderAPIDialect::NONE;
	Scope<Platform> Platform::sPlatform = nullptr;
	Scope<Window> Platform::sWindow = nullptr;
	Scope<Input> Platform::sInput = nullptr;

	bool Platform::SetupPlatform(uint32_t width,
								 uint32_t height,
								 std::string title,
								 RenderAPIDialect api)
	{
#ifdef ANTOMIC_PLATFORM_WINDOWS

		ANTOMIC_ASSERT(!sPlatform, "Platform: Platform already created!");

		sPlatform = CreateScope<WindowsPlatform>();
		ANTOMIC_ASSERT(!sRenderAPI, "Platform: Render API already set!")

		sRenderAPI = RenderAPI::Create(api);
		sRenderAPIDialect = RenderAPIDialect::OPENGL;

		sWindow = sPlatform->CreateWindow(width, height, title, api);
		if(!sWindow)
		{
			ANTOMIC_INFO("Platform: Unable to create window!");
			return false;
		}

		sInput = sPlatform->CreateInput();
		if(!sInput)
		{
			ANTOMIC_INFO("Platform: Unable to setup input!");
			return false;
		}

		return true;

#elif ANTOMIC_PLATFORM_LINUX

		ANTOMIC_ASSERT(!sPlatform, "Platform: Platform already created!");

		sPlatform = CreateScope<LinuxPlatform>();

		sRenderAPI = RenderAPI::Create(api);
		sRenderAPIDialect = RenderAPIDialect::OPENGL;

		sWindow = sPlatform->CreateWindow(width, height, title, api);
		if(!sWindow)
		{
			ANTOMIC_INFO("Platform: Unable to create window!");
			return false;
		}

		sInput = sPlatform->CreateInput();
		if(!sInput)
		{
			ANTOMIC_INFO("Platform: Unable to setup input!");
			return false;
		}

		return true;

#else
		ANTOMIC_ASSERT(false, "Platform: Platform not supported!");

		sPlatform = nullptr;
		sRenderAPI = nullptr;
		sWindow = nullptr;
		sInput = nullptr;

		return false;
#endif
	}
} // namespace Antomic
