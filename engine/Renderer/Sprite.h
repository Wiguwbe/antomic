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
    class Sprite : public Drawable
    {
    public:
        Sprite();
        virtual ~Sprite() override {}

    public:
        virtual const DrawableType GetType() override { return DrawableType::SPRITE; }
        virtual void Draw() override;

        inline const glm::vec4 &GetSpriteColor() const { return mSpriteColor; }
        inline void SetSpriteColor(const glm::vec4 &color) { mSpriteColor = color; }

    private:
        Ref<VertexArray> mVertexArray = nullptr;
        Ref<Shader> mShader = nullptr;
        glm::vec4 mSpriteColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
    };
}