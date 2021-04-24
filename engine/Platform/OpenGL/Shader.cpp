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
#include <glm/gtc/type_ptr.hpp>

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

        ANTOMIC_ASSERT(false, "ShaderDataTypeGLEnum: Unknown data type")
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

        ANTOMIC_ASSERT(false, "ShaderDataTypeGLSize: Unknown data type")
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
            ANTOMIC_ERROR("{0}", infoLog.data());
            ANTOMIC_ASSERT(false, "Vertex shader compilation error!");
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

            ANTOMIC_ERROR("{0}", infoLog.data());
            ANTOMIC_ASSERT(false, "Fragment shader compilation error!");
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

            ANTOMIC_ERROR("{0}", infoLog.data());
            ANTOMIC_ASSERT(false, "Shader Program link  error!");
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

    void OpenGLShader::SetUniformValue(const std::string &name, float value)
    {
        glUseProgram(mRendererId);
        GLint loc = glGetUniformLocation(mRendererId, name.c_str());
        if (loc != -1)
        {
            glUniform1f(loc, value);
        }
        glUseProgram(0);
    }

    void OpenGLShader::SetUniformValue(const std::string &name, const glm::vec2 &value)
    {
        glUseProgram(mRendererId);
        GLint loc = glGetUniformLocation(mRendererId, name.c_str());
        if (loc != -1)
        {
            glUniform2f(loc, value.x, value.y);
        }
        glUseProgram(0);
    }

    void OpenGLShader::SetUniformValue(const std::string &name, const glm::vec3 &value)
    {
        glUseProgram(mRendererId);
        GLint loc = glGetUniformLocation(mRendererId, name.c_str());
        if (loc != -1)
        {
            glUniform3f(loc, value.x, value.y, value.z);
        }
        glUseProgram(0);
    }

    void OpenGLShader::SetUniformValue(const std::string &name, const glm::vec4 &value)
    {
        glUseProgram(mRendererId);
        GLint loc = glGetUniformLocation(mRendererId, name.c_str());
        if (loc != -1)
        {
            glUniform4f(loc, value.x, value.y, value.z, value.w);
        }
        glUseProgram(0);
    }

    void OpenGLShader::SetUniformValue(const std::string &name, const glm::mat3 &value)
    {
        glUseProgram(mRendererId);
        GLint loc = glGetUniformLocation(mRendererId, name.c_str());
        if (loc != -1)
        {
            glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        }
        glUseProgram(0);
    }

    void OpenGLShader::SetUniformValue(const std::string &name, const glm::mat4 &value)
    {
        glUseProgram(mRendererId);
        GLint loc = glGetUniformLocation(mRendererId, name.c_str());
        if (loc != -1)
        {
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        }
        glUseProgram(0);
    }

} // namespace Antomic