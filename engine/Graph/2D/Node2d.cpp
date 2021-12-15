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
#include "Core/Log.h"
#include "Core/Serialization.h"
#include "Profiling/Instrumentor.h"
#include "Renderer/Drawable.h"
#include "Renderer/RendererFrame.h"
#include "Renderer/Sprite.h"

#include <glm/gtx/matrix_transform_2d.hpp>
namespace Antomic
{
	const glm::mat3& Node2d::GetWorldMatrix()
	{
		UpdateSpatialInformation();
		return mWorld;
	}

	const glm::mat3& Node2d::GetLocalMatrix()
	{
		UpdateSpatialInformation();
		return mLocal;
	}

	void Node2d::SetPosition(const glm::vec2& position)
	{
		mPosition = position;
		MakeDirty();
	}

	void Node2d::SetSize(const glm::vec2& size)
	{
		mSize = size;
		MakeDirty();
	}

	void Node2d::SetRotation(float rotation)
	{
		mRotation = rotation;
		MakeDirty();
	}

	void Node2d::SetAnchor(const glm::vec2& anchor)
	{
		mAnchor = glm::normalize(anchor);
		MakeDirty();
	}

	void Node2d::SetZOrder(int zorder) { mZOrder = zorder; }

	void Node2d::UpdateSpatialInformation()
	{
		if(!IsDirty())
		{
			return;
		}

		// Update Local Matrix
		mLocal = glm::mat4(1.0f);
		mLocal = glm::translate(mLocal, mPosition);
		mLocal = glm::rotate(mLocal, glm::radians(mRotation));
		mLocal = glm::translate(mLocal, {mSize.x * -abs(mAnchor.x), mSize.y * -abs(mAnchor.y)});
		mLocal = glm::scale(mLocal, mSize);

		// Update World Matrix
		auto parent = std::dynamic_pointer_cast<Node2d>(GetParent());
		mWorld = (parent == nullptr) ? mLocal : parent->GetWorldMatrix() * mLocal;

		// Update Model Matrix
		// Convert the 3x3 matrix to a 4x4 matrix
		GetDrawable()->SetModelMatrix(glm::mat4(glm::vec4(glm::vec2(mWorld[0]), 0, 0),
												glm::vec4(glm::vec2(mWorld[1]), 0, 0),
												glm::vec4(0, 0, 1, 0),
												glm::vec4(glm::vec2(mWorld[2]), 0, 1)));

		ClearDirty();
	}

	// Serialization
	void Node2d::Serialize(nlohmann::json& json)
	{
		json["type"] = "Node2d";
		Serialization::Serialize(json["position"], mPosition);
		Serialization::Serialize(json["size"], mSize);
		Serialization::Serialize(json["anchor"], mAnchor);
		json["rotation"] = mRotation;
		json["zorder"] = mZOrder;
		Node::Serialize(json);
	}
} // namespace Antomic
