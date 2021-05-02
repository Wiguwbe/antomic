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
#include "Graph/3D/Node3d.h"
#include "Renderer/Drawable.h"
#include "Renderer/Mesh.h"
#include "Renderer/RendererFrame.h"
#include "Core/Log.h"
#include "Profiling/Instrumentor.h"

namespace Antomic
{
    const glm::mat4 &Node3d::GetWorldMatrix()
    {
        if (IsDirty())
        {
            auto parent = std::dynamic_pointer_cast<Node3d>(GetParent());
            mWorld = (parent == nullptr) ? mLocal : parent->GetWorldMatrix() * mLocal;
            ClearDirty();
        }
        return mWorld;
    }

    void Node3d::SetLocalMatrix(const glm::mat4 &matrix)
    {
        mLocal = matrix;
        MakeDirty();
    }

    void Node3d::Update(const uint32_t &time)
    {
        // Update world matrix if dirty
        if (IsDirty())
        {
            auto parent = std::dynamic_pointer_cast<Node3d>(GetParent());
            mWorld = (parent == nullptr) ? mLocal : parent->GetWorldMatrix() * mLocal;
        }
        Node::Update(time);
    }

    void Node3d::SubmitDrawables(const Ref<RendererFrame> &frame)
    {
        // TODO: Optimize in order only to send drawables that are inside the view

        ANTOMIC_PROFILE_FUNCTION("Graph");

        // Update the current drawable matrix and submit all drawables in this node
        for (auto drawable : GetDrawables())
        {
            auto mesh = std::dynamic_pointer_cast<Mesh>(drawable);
            mesh->SetModelMatrix(mWorld);
            frame->QueueDrawable(mesh);
        }
        
        Node::SubmitDrawables(frame);
    }
}
