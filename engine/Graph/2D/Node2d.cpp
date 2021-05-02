/*
   Copyright 2020 Alexandre Pires (c.alexandre.pires@gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

c       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "Graph/2D/Node2d.h"
#include "Renderer/Sprite.h"
#include "Renderer/Drawable.h"
#include "Renderer/RendererFrame.h"
#include "Core/Log.h"
#include "Profiling/Instrumentor.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

namespace Antomic
{
    const glm::mat3 &Node2d::GetWorldMatrix()
    {
        UpdateSpatialInformation();
        return mWorld;
    }

    const glm::mat3 &Node2d::GetLocalMatrix()
    {
        UpdateSpatialInformation();
        return mLocal;
    }

    void Node2d::SetPosition(const glm::vec2 &position)
    {
        mPosition = position;
        MakeDirty();
    }

    void Node2d::SetSize(const glm::vec2 &size)
    {
        mSize = size;
        MakeDirty();
    }

    void Node2d::SetRotation(float rotation)
    {
        mRotation = rotation;
        MakeDirty();
    }

    void Node2d::UpdateSpatialInformation()
    {
        if (!IsDirty())
        {
            return;
        }

        // Update Local Matrix
        mLocal = glm::mat4(1.0f);
        mLocal = glm::translate(mLocal, mPosition);
        mLocal = glm::rotate(mLocal, glm::radians(mRotation));
        mLocal = glm::scale(mLocal, mSize);

        // Update World Matrix
        auto parent = std::dynamic_pointer_cast<Node2d>(GetParent());
        mWorld = (parent == nullptr) ? mLocal : parent->GetWorldMatrix() * mLocal;

        // TODO: Check if there is a way to take advantage of the mWorld matrix
        // auto model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(mPosition, 0.0f));
        // model = glm::translate(model, glm::vec3(0.5f * mSize.x, 0.5f * mSize.y, 0.0f));
        // model = glm::rotate(model, glm::radians(mRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        // model = glm::translate(model, glm::vec3(-0.5f * mSize.x, -0.5f * mSize.y, 0.0f));
        // model = glm::scale(model, glm::vec3(mSize, 1.0f));

        // Update Model Matrix
        auto mat43 = glm::vec4(glm::vec2(mWorld[2]), 0, 1);
        auto mat42 = glm::vec4(0, 0, 1, 0);
        auto mat41 = glm::vec4(glm::vec2(mWorld[1]), 0, 0);
        auto mat40 = glm::vec4(glm::vec2(mWorld[0]), 0, 0);
        auto model = glm::mat4(mat40, mat41, mat42, mat43);

        GetDrawable()->SetModelMatrix(model);

        ClearDirty();
    }
}
