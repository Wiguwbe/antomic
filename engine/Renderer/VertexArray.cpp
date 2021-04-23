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
#include "Core/Log.h"
#include "Renderer/Buffers.h"
#include "Platform/RendererAPI.h"

#ifdef ENGINE_GL_RENDERER
#include "Platform/OpenGL/VertexArray.h"
#endif

namespace Antomic
{
    Ref<VertexArray> VertexArray::Create()
    {
        switch (RendererAPI::API())
        {
        case RenderAPI::OPENGL:
#ifdef ENGINE_GL_RENDERER
            return CreateRef<OpenGLVertexArray>();
#else
            ENGINE_ASSERT(false, "VertexArray: OpenGL not available!");
            return nullptr;
#endif
            break;

        default:
            ENGINE_ASSERT(false, "VertexArray: No API available!");
            return nullptr;
            break;
        }
    }

}