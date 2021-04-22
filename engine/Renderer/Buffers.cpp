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

#ifdef ENGINE_GL_RENDERER
#include "Platform/OpenGL/Buffers.h"
#endif

namespace Engine
{
    Ref<IndexBuffer> IndexBuffer::CreateBuffer(uint32_t* data, uint32_t size)
	{
#ifdef ENGINE_GL_RENDERER
        return CreateRef<OpenGLIndexBuffer>(data, size);
#else
        ENGINE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

    Ref<VertexBuffer> VertexBuffer::CreateBuffer(float* data, uint32_t size)
	{
#ifdef ENGINE_GL_RENDERER
        return CreateRef<OpenGLVertexBuffer>(data, size);
#else
        ENGINE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

} 