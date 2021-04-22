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
#include "Renderer/Shader.h"

#ifdef ENGINE_GL_RENDERER
#include "Platform/OpenGL/Shader.h"
#endif

namespace Engine
{
    Ref<Shader> CreateFromFile(const std::string &vertexSrcPath, const std::string &pixelSrcPath)
    {
        std::string vertexSrc, pixelSrc;

        if (!std::filesystem::exists(vertexSrcPath))
        {
            ENGINE_ERROR("Shader: Could not find vertex shader file {0}", vertexSrcPath);
            ENGINE_ASSERT(false, "Vertex shader file not found!");
            return nullptr;
        }

        if (!std::filesystem::exists(pixelSrcPath))
        {
            ENGINE_ERROR("Shader: Could not find pixel shader file {0}", pixelSrcPath);
            ENGINE_ASSERT(false, "Pixel shader file not found!");
            return nullptr;
        }

        std::ifstream vertexSrcFile(vertexSrcPath);
        vertexSrcFile >> vertexSrc;

        std::ifstream pixelSrcFile(pixelSrcPath);
        pixelSrcFile >> pixelSrc;

#ifdef ENGINE_GL_RENDERER
        return CreateRef<OpenGLShader>(vertexSrc, pixelSrc);
#else
        ENGINE_ASSERT(false, "Unknown platform!");
        return nullptr;
#endif
    }

    Ref<Shader> CreateFromSource(const std::string &vertexSrc, const std::string &pixelSrc)
    {
#ifdef ENGINE_GL_RENDERER
        return CreateRef<OpenGLShader>(vertexSrc, pixelSrc);
#else
        ENGINE_ASSERT(false, "Unknown platform!");
        return nullptr;
#endif
    }

}