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
#include "Renderer/Material.h"
#include "glm/glm.hpp"

namespace Antomic
{
    class Mesh : public Drawable
    {
    public:
        Mesh(const Ref<VertexArray> &vertexArray, const Ref<Material> &material);
        virtual ~Mesh() = default;

    public:
        virtual const Ref<VertexArray> &GetVertexArray() const override { return mVertexArray; }
        virtual const glm::mat4 &GetModelMatrix() const override { return mMatrix; }
        virtual const VectorRef<Bindable> &GetBindables() const override { return mBindables; }
        virtual const Ref<Shader> &GetShader() const override { return mMaterial->GetShader(); }

        void SetMatrix(const glm::mat4 &matrix) { mMatrix = matrix; }
        void AddBindable(const Ref<Texture> &texture);
        void AddBindable(const Ref<Material> &material);

    private:
        Ref<VertexArray> mVertexArray;
        VectorRef<Bindable> mBindables;
        Ref<Material> mMaterial;
        glm::mat4 mMatrix;
    };
} // namespace Antomic