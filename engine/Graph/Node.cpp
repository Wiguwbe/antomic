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
        ANTOMIC_ASSERT(node != nullptr, "Node::AddChild: Child cannot be null");
        ANTOMIC_ASSERT(node->mParent.get() != this, "Node::AddChild: Already child of this node");

        // Scene accept any node type
        // Node2d only accepts Node2d
        // Node3d only accepts Node3d

        if (GetType() == NodeType::SCENE )
        {
            node->mParent = shared_from_this();
            mChildren.push_back(node);
            return;
        }

        if (node->GetType() != GetType())
        {
            ANTOMIC_INFO("Node::AddChild: Invalid node type. must be of the same type");
            return;
        }

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
        // Requests all childs to update
        ClearDirty();
        for (auto child : mChildren)
        {
            child->Update(time);
        }
    }

    void Node::SubmitDrawables(const Ref<RendererFrame> &frame)
    {
        // Asks children to submit alls drawables to frame
        for (auto child : mChildren)
        {
            child->SubmitDrawables(frame);
        }
    }

    void Node::AddDrawable(const Ref<Drawable> &drawable)
    {
        switch (GetType())
        {
        case NodeType::NODE_2D:
            // Node2d only accept sprites
            if (drawable->GetType() != DrawableType::SPRITE)
            {
                ANTOMIC_ASSERT(false, "Node::AddDrawable: Invalid type");
                return;
            }
            mDrawables.push_back(drawable);
            return;
        case NodeType::NODE_3D:
            // Node3d only accept meshes
            if (drawable->GetType() != DrawableType::MESH)
            {
                ANTOMIC_ASSERT(false, "Node::AddDrawable: Invalid type");
                return;
            }
            mDrawables.push_back(drawable);
            return;
        case NodeType::SCENE:
            // Don't accept any
            return;
        default:
            ANTOMIC_ASSERT(false, "Node::AddDrawable: Invalid type");
            return;
        }
    }
}
