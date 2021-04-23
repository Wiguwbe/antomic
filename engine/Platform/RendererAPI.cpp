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
#include "Platform/RendererAPI.h"
#include "Core/Log.h"

#ifdef ENGINE_GL_RENDERER
#include "Platform/OpenGL/RendererAPI.h"
#endif


namespace Antomic {
    RenderAPI RendererAPI::sRendererAPI = RenderAPI::NONE;

	Scope<RendererAPI> RendererAPI::Create(RenderAPI api)
	{
        switch (api)
        {
        case RenderAPI::OPENGL:
#ifdef ENGINE_GL_RENDERER
            ENGINE_INFO("Renderer: Using OpenGL renderer API");
            sRendererAPI = RenderAPI::OPENGL;
            return CreateScope<OpenGLRendererAPI>();
#else
            ENGINE_ASSERT(false, "Renderer: OpenGL not available!");
            sRendererAPI = RenderAPI::NONE;
		    return nullptr;
#endif
            break;        
        default:
            ENGINE_ASSERT(false, "Renderer: API implemented yet!");
            sRendererAPI = RenderAPI::NONE;
		    return nullptr;
            break;
        }
	}

}
