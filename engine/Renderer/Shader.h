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
#include "Core/Base.h"
#include "Renderer/Bindable.h"
#include "glm/glm.hpp"

namespace Antomic
{
    enum class ShaderDataType
    {
        Float,
        Float2,
        Float3,
        Float4,
        Mat3,
        Mat4,
        Int,
        Int2,
        Int3,
        Int4,
        Bool
    };

    uint32_t ShaderDataTypeSize(ShaderDataType t);

    class Shader : public Bindable
    {
    public:
        virtual ~Shader() = default;

    public:

        // Uniform Commands
        virtual void SetUniformValue(const std::string& name, float value) = 0;
        virtual void SetUniformValue(const std::string& name, const glm::vec2 &value) = 0;
        virtual void SetUniformValue(const std::string& name, const glm::vec3 &value) = 0;
        virtual void SetUniformValue(const std::string& name, const glm::vec4 &value) = 0;
        virtual void SetUniformValue(const std::string& name, const glm::mat3 &value) = 0;
        virtual void SetUniformValue(const std::string& name, const glm::mat4 &value) = 0;

    public:
        static Ref<Shader> CreateFromFile(const std::string &vertexSrcPath, const std::string &pixelSrcPath);
        static Ref<Shader> CreateFromSource(const std::string &vertexSrc, const std::string &pixelSrc);
    };

} // namespace Antomic
