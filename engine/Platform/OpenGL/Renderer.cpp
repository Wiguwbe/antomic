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
#include "Platform/OpenGL/Renderer.h"
#include <glad/glad.h>

namespace Antomic
{

    void OpenGLRenderer::Init(uint32_t width, uint32_t height)
    {
        OnWindowResize(width, height);
    }

    void OpenGLRenderer::Shutdown()
    {
    }

    void OpenGLRenderer::OnWindowResize(uint32_t width, uint32_t height)
    {
    }

    void OpenGLRenderer::RenderFrame()
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void OpenGLRenderer::BeginScene()
    {
    }

    void OpenGLRenderer::EndScene()
    {
    }

} // namespace Antomic