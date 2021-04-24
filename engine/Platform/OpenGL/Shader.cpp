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
#include "Platform/OpenGL/Shader.h"
#include "Core/Log.h"
#include <glad/glad.h>

namespace Antomic
{
    GLenum ShaderDataTypeGLEnum(ShaderDataType t)
    {
        switch (t)
        {
        case ShaderDataType::Float:
        case ShaderDataType::Float2:
        case ShaderDataType::Float3:
        case ShaderDataType::Float4:
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4:
            return GL_FLOAT;
        case ShaderDataType::Int:
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
            return GL_INT;
        case ShaderDataType::Bool:
            return GL_UNSIGNED_SHORT;
        }

        ENGINE_ASSERT(false, "ShaderDataTypeGLEnum: Unknown data type")
        return GL_UNSIGNED_SHORT;
    }

    GLint ShaderDataTypeGLSize(ShaderDataType t)
    {
        switch (t)
        {
        case ShaderDataType::Float:
            return 1;
        case ShaderDataType::Float2:
            return 2;
        case ShaderDataType::Float3:
            return 3;
        case ShaderDataType::Float4:
            return 4;
        case ShaderDataType::Mat3:
            return 9;
        case ShaderDataType::Mat4:
            return 16;
        case ShaderDataType::Int:
            return 1;
        case ShaderDataType::Int2:
            return 2;
        case ShaderDataType::Int3:
            return 3;
        case ShaderDataType::Int4:
            return 4;
        case ShaderDataType::Bool:
            return 1;
        }

        ENGINE_ASSERT(false, "ShaderDataTypeGLSize: Unknown data type")
        return GL_UNSIGNED_SHORT;
    }

    OpenGLShader::OpenGLShader(const std::string &vertexSrc, const std::string &fragmentSrc)
    {
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const GLchar *source = vertexSrc.c_str();
        glShaderSource(vertexShader, 1, &source, 0);

        glCompileShader(vertexShader);
        GLint isCompiled = 0;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(vertexShader);
            ENGINE_ERROR("{0}", infoLog.data());
            ENGINE_ASSERT(false, "Vertex shader compilation error!");
            return;
        }

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        source = fragmentSrc.c_str();
        glShaderSource(fragmentShader, 1, &source, 0);

        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(fragmentShader);
            glDeleteShader(vertexShader);

            ENGINE_ERROR("{0}", infoLog.data());
            ENGINE_ASSERT(false, "Fragment shader compilation error!");
            return;
        }

        mRendererId = glCreateProgram();

        glAttachShader(mRendererId, vertexShader);
        glAttachShader(mRendererId, fragmentShader);

        glLinkProgram(mRendererId);

        GLint isLinked = 0;
        glGetProgramiv(mRendererId, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(mRendererId, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(mRendererId, maxLength, &maxLength, &infoLog[0]);

            glDeleteProgram(mRendererId);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            ENGINE_ERROR("{0}", infoLog.data());
            ENGINE_ASSERT(false, "Shader Program link  error!");
            return;
        }

        // Always detach shaders after a successful link.
        glDetachShader(mRendererId, vertexShader);
        glDetachShader(mRendererId, fragmentShader);
    }

    OpenGLShader::~OpenGLShader()
    {
        glDeleteProgram(mRendererId);
    }

    void OpenGLShader::Bind() const
    {
        glUseProgram(mRendererId);
    }

    void OpenGLShader::Unbind() const
    {
        glUseProgram(0);
    }

} // namespace Antomic