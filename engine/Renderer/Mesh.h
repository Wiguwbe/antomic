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
#include "Renderer/Drawable.h"
#include "glm/glm.hpp"

namespace Antomic
{
    class Mesh : public Drawable
    {
    public:
        Mesh(const Ref<VertexArray> &vertexArray, const Ref<Material> &material);
        virtual ~Mesh() override {};

    public:
        virtual const glm::mat4 &GetModelMatrix() const override { return mMatrix; }
        virtual void SetModelMatrix(const glm::mat4 &matrix) override { mMatrix = matrix; }
        virtual const DrawableType GetType() override { return DrawableType::MESH; }
        virtual void Draw() override;

    private:
        Ref<VertexArray> mVertexArray;
        Ref<Material> mMaterial;
        glm::mat4 mMatrix;  
    };
}