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
#include "glm/glm.hpp"

namespace Antomic
{
    class Node : public std::enable_shared_from_this<Node>
    {
    public:
        Node() : mParent(nullptr), mLocal(1.0f),mWorld(1.0f) {};
        virtual ~Node() = default;

    public:

        // Spatial Operations
        inline const glm::mat4 &GetLocalMatrix() { return mLocal; }
        const glm::mat4 &GetWorldMatrix();
        void SetLocalMatrix(const glm::mat4 &matrix);

        // Graph Operations
        inline const VectorRef<Node> &GetChildren() const { return mChildren; }
        inline const Ref<Node> &GetParent() const { return mParent; }    
        void AddChild(const Ref<Node> &node);
        void RemoveChild(const Ref<Node> &node);

        // Container Operations
        void AddDrawable(const Ref<Drawable> &drawable) { mDrawables.push_back(drawable); }

        // Render Operations
        virtual void SubmitDrawables(const Ref<RendererFrame> &frame);
        
        // State Operations
        virtual void Update(const uint32_t &time);

    protected:
        void MakeDirty();

    private:
        Ref<Node> mParent;
        glm::mat4 mLocal, mWorld;
        bool mDirty;
        VectorRef<Node> mChildren;    
        VectorRef<Drawable> mDrawables;
    };
} // namespace Antomic
