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
    class Node3d : public Node
    {
    public:
        Node3d() = default;
        virtual ~Node3d() = default;

    public:
        // Graph Operations
        virtual NodeType GetType() override { return NodeType::NODE_3D; };

        // Spatial Operations
        const glm::mat4 &GetWorldMatrix();
        inline const glm::mat4 &GetLocalMatrix() { return mLocal; }
        void SetLocalMatrix(const glm::mat4 &matrix);

        // Render Operations
        virtual void SubmitDrawables(const Ref<RendererFrame> &frame);

        // State Operations
        virtual void Update(const uint32_t &time) override;

    private:
        glm::mat4 mLocal = glm::mat4(1.0f);
        glm::mat4 mWorld = glm::mat4(1.0f);
    };
} // namespace Antomic
