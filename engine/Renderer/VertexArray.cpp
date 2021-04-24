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
#include "Platform/NullRenderer/VertexArray.h"
#include "Platform/Platform.h"
#ifdef ANTOMIC_GL_RENDERER
#include "Platform/OpenGL/VertexArray.h"
#endif

namespace Antomic
{
    Ref<VertexArray> VertexArray::Create()
    {
        switch (Platform::GetRenderAPIDialect())
        {
#ifdef ANTOMIC_GL_RENDERER
        case RenderAPI::OPENGL:
            return CreateRef<OpenGLVertexArray>();
#endif
        default:
            return CreateRef<NullVertexArray>();
        }
    }

}