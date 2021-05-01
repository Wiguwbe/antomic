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
#include "Graph/Node.h"
#include "Renderer/Drawable.h"
#include "Renderer/RendererFrame.h"
#include "Core/Log.h"
#include "Profiling/Instrumentor.h"

namespace Antomic
{
    void Node::AddChild(const Ref<Node> &node)
    {
        ANTOMIC_ASSERT(node != nullptr, "Node: Child cannot be null");
        ANTOMIC_ASSERT(node->mParent.get() != this, "Node: Already child of this node");
        node->mParent = shared_from_this();
        mChildren.push_back(node);
    }

    void Node::RemoveChild(const Ref<Node> &node)
    {
        ANTOMIC_ASSERT(node != nullptr, "Node: Child cannot be null");
        ANTOMIC_ASSERT(node->mParent.get() == this, "Node: Not a child of this node");
        auto child = std::find(mChildren.begin(), mChildren.end(), node);
        (*child)->mParent = nullptr;
        mChildren.erase(child);
    }

    const glm::mat4 &Node::GetWorldMatrix()
    {
        if (mDirty)
        {
            mWorld = GetParent() == nullptr ? mLocal : GetParent()->GetWorldMatrix() * mLocal;
            mDirty = false;
        }
        return mWorld;
    }

    void Node::SetLocalMatrix(const glm::mat4 &matrix)
    {
        mLocal = matrix;
        MakeDirty();
    }

    void Node::MakeDirty()
    {
        mDirty = true;
        for (auto child : mChildren)
        {
            child->MakeDirty();
        }
    }

    void Node::Update(const uint32_t &time)
    {
        // Update world matrix if dirty
        if (mDirty)
        {
            mWorld = GetParent() == nullptr ? mLocal : GetParent()->GetWorldMatrix() * mLocal;
            mDirty = false;
        }

        // Update the view matrix in all attached drawables
        for (auto drawable : mDrawables)
        {
            drawable->SetMatrix(mWorld);
        }

        // Requests all childs to update
        for (auto child : mChildren)
        {
            child->Update(time);
        }
    }

    void Node::SubmitDrawables(const Ref<RendererFrame> &frame, const glm::mat4 &view)
    {
        // TODO: Optimize in order only to send drawables that are inside the view

        ANTOMIC_PROFILE_FUNCTION();

        // Submit all drawables in this node
        for (auto drawable : mDrawables)
        {
            frame->QueueDrawable(drawable);
        }

        // Asks children to submit alls drawables to frame
        for (auto child : mChildren)
        {
            child->SubmitDrawables(frame, view);
        }
    }

}
