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
#include "Graph/2D/Node2d.h"
#include "Renderer/Sprite.h"
#include "Renderer/Drawable.h"
#include "Renderer/RendererFrame.h"
#include "Core/Log.h"
#include "Profiling/Instrumentor.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Antomic
{
    const glm::mat3 &Node2d::GetWorldMatrix()
    {
        if (IsDirty())
        {
            auto parent = std::dynamic_pointer_cast<Node2d>(GetParent());
            mWorld = (parent == nullptr) ? mLocal : parent->GetWorldMatrix() * mLocal;
            ClearDirty();
        }
        return mWorld;
    }

    void Node2d::SetLocalMatrix(const glm::mat3 &local)
    {
        mLocal = local;
        MakeDirty();
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

    void Node2d::SetColor(const glm::vec4 &color)
    {
        mColor = color;
    }

    void Node2d::Update(const uint32_t &time)
    {
        // Update world matrix if dirty
        if (IsDirty())
        {
            auto parent = std::dynamic_pointer_cast<Node2d>(GetParent());
            mWorld = (parent == nullptr) ? mLocal : parent->GetWorldMatrix() * mLocal;
            mModelMatrix = glm::mat4(1.0f);
            mModelMatrix = glm::translate(mModelMatrix, glm::vec3(mPosition, 0.0f));  
            mModelMatrix = glm::translate(mModelMatrix, glm::vec3(0.5f * mSize.x, 0.5f * mSize.y, 0.0f)); 
            mModelMatrix = glm::rotate(mModelMatrix, glm::radians(mRotation), glm::vec3(0.0f, 0.0f, 1.0f)); 
            mModelMatrix = glm::translate(mModelMatrix, glm::vec3(-0.5f * mSize.x, -0.5f * mSize.y, 0.0f));
            mModelMatrix = glm::scale(mModelMatrix, glm::vec3(mSize, 1.0f)); 
        }
        Node::Update(time);
    }

    void Node2d::SubmitDrawables(const Ref<RendererFrame> &frame)
    {
        // TODO: Optimize in order only to send drawables that are inside the view

        ANTOMIC_PROFILE_FUNCTION("Graph");

        // Update the current drawable matrix and submit all drawables in this node
        for (auto drawable : GetDrawables())
        {
            auto sprite = std::dynamic_pointer_cast<Sprite>(drawable);
            sprite->SetModelMatrix(mModelMatrix);
            sprite->SetSpriteColor(mColor);
            frame->QueueDrawable(sprite);
        }
        
        Node::SubmitDrawables(frame);
    }
}
