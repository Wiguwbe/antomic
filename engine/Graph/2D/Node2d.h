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
#include "Graph/Node.h"
#include "glm/glm.hpp"

namespace Antomic
{
    class Node2d : public Node
    {
    public:
        Node2d() = default;
        virtual ~Node2d() = default;

    public:
        // Graph operations
        virtual NodeType GetType() override { return NodeType::NODE_2D; };

        // Spatial Operations
        const glm::mat3 &GetWorldMatrix();
        inline const glm::mat3 &GetLocalMatrix() { return mLocal; }
        void SetLocalMatrix(const glm::mat3 &transform);

        // Spatial Operation
        inline const glm::vec2 &getPosition() const { return mPosition; }
        inline const glm::vec2 &getSize() const { return mSize; }
        inline const float &getRotation() const { return mRotation; }
        inline const glm::vec4 &getColor() const { return mColor; }

        void SetPosition(const glm::vec2 &position);
        void SetSize(const glm::vec2 &size);
        void SetRotation(float rotation);
        void SetColor(const glm::vec4 &color);

        inline int GetZOrder() { return mZOrder; }
        inline void SetZOrder(int zorder) { mZOrder = zorder; }

        // Render Operations
        virtual void SubmitDrawables(const Ref<RendererFrame> &frame);

        // State Operations
        virtual void Update(const uint32_t &time) override;

    private:
        glm::mat3 mLocal = glm::mat3(1.f);
        glm::mat3 mWorld = glm::mat3(1.f);
        glm::mat4 mModelMatrix = glm::mat4(1.f);
        glm::vec2 mPosition = glm::vec2(0,0);
        glm::vec2 mSize = glm::vec2(1,1);;
        glm::vec4 mColor = glm::vec4(1,1,1,1);
        float mRotation = 0.f;
        int mZOrder = 0.f;
    };
} // namespace Antomic
