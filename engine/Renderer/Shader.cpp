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
#include "Platform/RenderAPI.h"

#ifdef ENGINE_GL_RENDERER
#include "Platform/OpenGL/Shader.h"
#endif

namespace Antomic
{
    uint32_t ShaderDataTypeSize(ShaderDataType t)
    {
        switch (t)
        {
        case ShaderDataType::Float:
            return 4;
        case ShaderDataType::Float2:
            return 8;
        case ShaderDataType::Float3:
            return 12;
        case ShaderDataType::Float4:
            return 16;
        case ShaderDataType::Mat3:
            return 36;
        case ShaderDataType::Mat4:
            return 64;
        case ShaderDataType::Int:
            return 4;
        case ShaderDataType::Int2:
            return 8;
        case ShaderDataType::Int3:
            return 12;
        case ShaderDataType::Int4:
            return 16;
        case ShaderDataType::Bool:
            return 1;
        }

        ENGINE_ASSERT(false, "ShaderDataType: Unknown data tyepe")
        return 0;
    }

    Ref<Shader> Shader::CreateFromFile(const std::string &vertexSrcPath, const std::string &pixelSrcPath)
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
        vertexSrc = std::string((std::istreambuf_iterator<char>(vertexSrcFile)),
                                std::istreambuf_iterator<char>());

        std::ifstream pixelSrcFile(pixelSrcPath);
        pixelSrc = std::string((std::istreambuf_iterator<char>(pixelSrcFile)),
                               std::istreambuf_iterator<char>());

        return CreateFromSource(vertexSrc, pixelSrc);
    }

    Ref<Shader> Shader::CreateFromSource(const std::string &vertexSrc, const std::string &pixelSrc)
    {
        switch (RenderAPI::CurrentAPI())
        {
        case RenderPlatform::OPENGL:
#ifdef ENGINE_GL_RENDERER
            return CreateRef<OpenGLShader>(vertexSrc, pixelSrc);
#else
            ENGINE_ASSERT(false, "Shader: OpenGL not available!");
            return nullptr;
#endif
            break;

        default:
            ENGINE_ASSERT(false, "Shader: No API available!");
            return nullptr;
            break;
        }
    }

}