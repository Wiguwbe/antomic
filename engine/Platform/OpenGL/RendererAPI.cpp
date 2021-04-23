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
#include "Platform/OpenGL/RendererAPI.h"
#include "glad/glad.h"

namespace Antomic
{
    OpenGLRendererAPI::OpenGLRendererAPI() 
    {

    }

    OpenGLRendererAPI::~OpenGLRendererAPI() 
    {

    }

    void OpenGLRendererAPI::Clear(glm::vec4 color)
    {
        glClearColor(color.r,color.g,color.b,color.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray> vertexArray)
    {

    }


} // namespace Antomic