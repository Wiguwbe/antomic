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
#include "Graph/2D/Node2d.h"
#include "Renderer/Sprite.h"
#include "glm/glm.hpp"

namespace Antomic
{
    class SpriteNode : public Node2d
    {
    public:
        SpriteNode(const std::string name);
        virtual ~SpriteNode() = default;

    public:
        // Object attributes
        inline const glm::vec4 &GetColor() const { return mSprite->GetSpriteColor(); }
        inline void SetColor(const glm::vec4 &color) { mSprite->SetSpriteColor(color); }

    public:
        // Serialization
        virtual void Serialize(nlohmann::json &json) override;
        virtual void Deserialize(const nlohmann::json &json) override;

    protected:
        virtual const Ref<Drawable> GetDrawable() const override { return mSprite; };

    private:
        std::string mUrl;
        Ref<Sprite> mSprite;
    };
} // namespace Antomic
