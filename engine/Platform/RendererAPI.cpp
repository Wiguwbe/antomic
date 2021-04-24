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
#include "Platform/NullRenderer/RendererAPI.h"
#ifdef ANTOMIC_GL_RENDERER
#include "Platform/OpenGL/RendererAPI.h"
#endif

namespace Antomic
{
    RenderAPI RendererAPI::sRendererAPI = RenderAPI::NONE;

    RenderAPI RenderAPIFromStr(const std::string &api)
    {

        if (api.compare("opengl") == 0)
            return RenderAPI::OPENGL;

        return RenderAPI::NONE;
    }

    std::string const RenderAPIToStr(RenderAPI api)
    {

        switch (api)
        {
        case RenderAPI::NONE:
            return "Null API";
        case RenderAPI::OPENGL:
            return "OpenGL";
        }

        ANTOMIC_ASSERT(false, "RenderAPI: Unknown api")
        return 0;
    }

    Scope<RendererAPI> RendererAPI::Create(RenderAPI api)
    {
        switch (api)
        {
#ifdef ANTOMIC_GL_RENDERER
        case RenderAPI::OPENGL:
            ANTOMIC_INFO("Renderer: Using OpenGL renderer API");
            sRendererAPI = RenderAPI::OPENGL;
            return CreateScope<OpenGLRendererAPI>();
#endif
        default:
            ANTOMIC_INFO("Renderer: Using Null renderer API!");
            sRendererAPI = RenderAPI::NONE;
            return CreateScope<NullRendererAPI>();
        }
    }

}
