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

        mGLProgramId = glCreateProgram();

        glAttachShader(mGLProgramId, vertexShader);
        glAttachShader(mGLProgramId, fragmentShader);

        glLinkProgram(mGLProgramId);

        GLint isLinked = 0;
        glGetProgramiv(mGLProgramId, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(mGLProgramId, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(mGLProgramId, maxLength, &maxLength, &infoLog[0]);

            glDeleteProgram(mGLProgramId);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            ENGINE_ERROR("{0}", infoLog.data());
            ENGINE_ASSERT(false, "Shader Program link  error!");
            return;
        }

        // Always detach shaders after a successful link.
        glDetachShader(mGLProgramId, vertexShader);
        glDetachShader(mGLProgramId, fragmentShader);
    }

    OpenGLShader::~OpenGLShader()
    {
        glDeleteProgram(mGLProgramId);
    }

    void OpenGLShader::Bind()
    {
        glUseProgram(mGLProgramId);
    }

    void OpenGLShader::Unbind()
    {
        glUseProgram(0);
    }

} // namespace Antomic