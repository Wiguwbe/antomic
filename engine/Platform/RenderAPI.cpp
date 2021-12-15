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
#include "Platform/RenderAPI.h"
#include "Core/Log.h"
#include "Platform/NullRenderer/RenderAPI.h"
#ifdef ANTOMIC_GL_RENDERER
#	include "Platform/OpenGL/RenderAPI.h"
#endif

namespace Antomic
{
	RenderAPIDialect RenderAPIFromStr(const std::string& api)
	{
		if(api.compare("opengl") == 0)
			return RenderAPIDialect::OPENGL;

		return RenderAPIDialect::NONE;
	}

	std::string const RenderAPIToStr(RenderAPIDialect api)
	{
		switch(api)
		{
			case RenderAPIDialect::NONE: return "Null API";
			case RenderAPIDialect::OPENGL: return "OpenGL";
		}

		ANTOMIC_ASSERT(false, "RenderAPIDialect: Unknown api")
		return 0;
	}

	Scope<RenderAPI> RenderAPI::Create(RenderAPIDialect api)
	{
		switch(api)
		{
#ifdef ANTOMIC_GL_RENDERER
			case RenderAPIDialect::OPENGL:
				ANTOMIC_INFO("Renderer: Using OpenGL renderer API");
				return CreateScope<OpenGLRenderAPI>();
#endif
			default:
				ANTOMIC_INFO("Renderer: Using Null renderer API!");
				return CreateScope<NullRenderAPI>();
		}
	}

} // namespace Antomic
