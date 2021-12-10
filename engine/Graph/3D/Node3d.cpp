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
#include "Core/Log.h"
#include "Profiling/Instrumentor.h"
#include "Renderer/Drawable.h"
#include "Renderer/Mesh.h"
#include "Renderer/RendererFrame.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

namespace Antomic
{
	const glm::mat4& Node3d::GetWorldMatrix()
	{
		UpdateSpatialInformation();
		return mWorld;
	}

	void Node3d::SetLocalMatrix(const glm::mat4& matrix)
	{
		mLocal = matrix;
		MakeDirty();
	}

	void Node3d::SetPosition(const glm::vec3& position)
	{
		mPosition = position;
		MakeDirty();
	}

	void Node3d::SetSize(const glm::vec3& size)
	{
		mSize = size;
		MakeDirty();
	}

	void Node3d::SetRotation(const glm::vec3& rotation)
	{
		mRotation = rotation;
		MakeDirty();
	}

	void Node3d::UpdateSpatialInformation()
	{
		if(!IsDirty())
		{
			return;
		}

		// Update the local matrix
		auto quat = glm::quat(mRotation);
		mLocal = glm::mat4(quat);
		mLocal = glm::translate(mLocal, mPosition);
		mLocal = glm::scale(mSize);

		// Update the world matrix
		auto parent = std::dynamic_pointer_cast<Node3d>(GetParent());
		mWorld = (parent == nullptr) ? mLocal : parent->GetWorldMatrix() * mLocal;

		// Set the model matrix as the world matrix
		GetDrawable()->SetModelMatrix(mWorld);
		ClearDirty();
	}

} // namespace Antomic
