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
#include "nlohmann/json.hpp"

namespace Antomic
{
    enum class NodeType
    {
        SCENE,
        NODE_3D,
        NODE_2D
    };

    class Node : public std::enable_shared_from_this<Node>
    {
    public:
        virtual ~Node() = default;

    public:
        // Graph Operations
        inline const VectorRef<Node> &GetChildren() const { return mChildren; }
        inline const Ref<Node> &GetParent() const { return mParent; }
        void AddChild(const Ref<Node> &node);
        void RemoveChild(const Ref<Node> &node);
        virtual NodeType GetType() = 0;

        // Render Operations
        virtual void SubmitDrawables(const Ref<RendererFrame> &frame);

        // State Operations
        virtual void Update(const uint32_t &time);

        // Serialization
        virtual void Serialize(nlohmann::json &json);
        
        static Ref<Node> Deserialize(const nlohmann::json &json);

    protected:
        // Render Operations
        virtual const Ref<Drawable> GetDrawable() const = 0;

        // State operations
        virtual void MakeDirty();
        inline bool IsDirty() { return mDirty; }
        inline void ClearDirty() { mDirty = false; }
        virtual void UpdateSpatialInformation() = 0;

    private:
        Ref<Node> mParent = nullptr;
        VectorRef<Node> mChildren;
        bool mDirty = true;
    };
} // namespace Antomic
