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
#include "Platform/OpenGL/RenderAPI.h"
#include "glad/glad.h"

namespace Antomic
{
    void OpenGLRenderAPI::SetViewport(const uint32_t &x, const uint32_t &y, uint32_t const &width, uint32_t const &height)
    {
        glViewport(x, y, width, height);
    }

    void OpenGLRenderAPI::SetClearColor(glm::vec4 color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void OpenGLRenderAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void OpenGLRenderAPI::DrawIndexed(const Ref<VertexArray> vertexArray)
    {
        vertexArray->Bind();
        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->Count(), GL_UNSIGNED_INT, nullptr);
    }

} // namespace Antomic