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
#pragma once
#include "Renderer/Shader.h"
#include "glad/glad.h"

namespace Antomic
{
    GLenum ShaderDataTypeGLEnum(ShaderDataType t);
    GLint  ShaderDataTypeGLSize(ShaderDataType t);

    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string &vertexSrc, const std::string &fragmentSrc);
        ~OpenGLShader();
        virtual void Bind() const override;
        virtual void Unbind() const override;

    private:
        uint32_t mRendererId = 0;
    };

} // namespace Antomic
