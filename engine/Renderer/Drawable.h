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
#include "Renderer/Material.h"
#include "glm/glm.hpp"

namespace Antomic
{
    enum class DrawableType
    {
        NONE,
        SPRITE,
        MESH
    };

    class Drawable
    {
    public:
        virtual ~Drawable() = default;

    public:
        virtual void Draw() = 0;
        virtual DrawableType const GetType() = 0;
        const glm::mat4 &GetModelMatrix() const { return mModelMatrix; }
        void SetModelMatrix(const glm::mat4 &matrix) { mModelMatrix = matrix; }

        inline void AddBindable(const Ref<Bindable> &bindable) { mBindables.push_back(bindable); }
        inline const VectorRef<Bindable> &GetBindables() const { return mBindables; }
        
    private:
        VectorRef<Bindable> mBindables;
        glm::mat4 mModelMatrix;
    };
} // namespace Antomic