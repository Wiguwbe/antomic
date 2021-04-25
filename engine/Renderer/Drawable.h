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
#include "glm/glm.hpp"

namespace Antomic
{
    class Drawable
    {
    public:
        Drawable(const Ref<VertexArray> &vertexArray, const Ref<Shader> &shader);
        virtual ~Drawable();

    public:
        const Ref<VertexArray> &GetVertexArray() const { return mVertexArray; }
        const Ref<Shader> &GetShader() const { return mShader; }
        const glm::mat4 &GetMatrix() const { return mMatrix; }
        void SetMatrix(const glm::mat4 &matrix) { mMatrix = matrix; }

    private:
        Ref<VertexArray> mVertexArray;
        Ref<Shader> mShader;
        glm::mat4 mMatrix;

    };
} // namespace Antomic