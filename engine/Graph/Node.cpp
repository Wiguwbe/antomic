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
#include "Core/Log.h"
#include "Graph/2D/SpriteNode.h"
#include "Profiling/Instrumentor.h"
#include "Renderer/Drawable.h"
#include "Renderer/RendererFrame.h"

namespace Antomic
{
	void Node::AddChild(const Ref<Node>& node)
	{
		ANTOMIC_ASSERT(node != nullptr, "Node::AddChild: Child cannot be null");
		ANTOMIC_ASSERT(node->mParent.get() != this, "Node::AddChild: Already child of this node");

		// Scene accept any node type
		// Node2d only accepts Node2d
		// Node3d only accepts Node3d

		if(GetType() == NodeType::SCENE)
		{
			node->mParent = shared_from_this();
			mChildren.push_back(node);
			return;
		}

		if(node->GetType() != GetType())
		{
			ANTOMIC_INFO("Node::AddChild: Invalid node type. must be of the same type");
			return;
		}

		node->mParent = shared_from_this();
		mChildren.push_back(node);
	}

	void Node::RemoveChild(const Ref<Node>& node)
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
		for(auto child : mChildren)
		{
			child->MakeDirty();
		}
	}

	void Node::CleanUp()
	{
		for(auto child : mChildren)
		{
			child->CleanUp();
		}
	}

	void Node::Update(const uint32_t& time)
	{
		// Requests all childs to update
		UpdateSpatialInformation();
		for(auto child : mChildren)
		{
			child->Update(time);
		}
	}

	void Node::SubmitDrawables(const Ref<RendererFrame>& frame)
	{
		// TODO: Optimize in order only to send drawables that are inside the view

		ANTOMIC_PROFILE_FUNCTION("Graph");

		// Submit any existing drawable if it exists
		if(GetDrawable() != nullptr)
		{
			frame->QueueDrawable(GetDrawable());
		}

		// Asks children to submit their drawables to frame
		for(auto child : mChildren)
		{
			child->SubmitDrawables(frame);
		}
	}

	void Node::Serialize(nlohmann::json& json)
	{
		for(auto child : mChildren)
		{
			auto node = nlohmann::json();
			child->Serialize(node);
			json["nodes"].push_back(node);
		}
	}

	Ref<Node> Node::Deserialize(const nlohmann::json& json)
	{
		Ref<Node> nodeRef = nullptr;

		auto nodeClass = json["class"].get<std::string>();
		if(nodeClass == "SpriteNode")
		{
			nodeRef = SpriteNode::Deserialize(json);
		}

		if(nodeRef != nullptr && json.contains("nodes"))
		{
			for(auto node : json["nodes"])
			{
				nodeRef->AddChild(Node::Deserialize(node));
			}
		}

		return nodeRef;
	}
} // namespace Antomic
