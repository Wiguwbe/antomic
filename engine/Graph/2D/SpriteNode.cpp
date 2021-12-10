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
#include "Graph/2D/SpriteNode.h"
#include "Core/Serialization.h"
#include "Profiling/Instrumentor.h"
#include "Renderer/RendererFrame.h"
#include "Renderer/Sprite.h"
#include "Renderer/Texture.h"
#include "stb_image.h"
#include <glm/gtx/matrix_transform_2d.hpp>

namespace Antomic
{
	SpriteNode::SpriteNode(const std::string name)
		: mUrl(name)
	{
		{
			mSprite = CreateRef<Sprite>();
			// TODO: change the way we load resources
			// load and generate the texture
			int width, height, nrChannels;
			unsigned char* data = stbi_load(name.c_str(), &width, &height, &nrChannels, 0);
			if(data)
			{
				Ref<Texture> texture = Texture::CreateTexture(width, height, data);
				mSprite->AddBindable(texture);
			}
			stbi_image_free(data);
		}
	}

	// Serialization
	void SpriteNode::Serialize(nlohmann::json& json)
	{
		json["class"] = "SpriteNode";
		json["url"] = mUrl;
		Node2d::Serialize(json);
	}

	Ref<SpriteNode> SpriteNode::Deserialize(const nlohmann::json& json)
	{
		glm::vec2 pos, size, anchor;
		ANTOMIC_ASSERT(json.contains("url"), "Missing sprite URL");
		auto url = json["url"].get<std::string>();
		ANTOMIC_ASSERT(json.contains("position"), "Missing sprite position");
		Serialization::Deserialize(json["position"], pos);
		ANTOMIC_ASSERT(json.contains("size"), "Missing sprite size");
		Serialization::Deserialize(json["size"], size);
		ANTOMIC_ASSERT(json.contains("anchor"), "Missing sprite anchor");
		Serialization::Deserialize(json["anchor"], anchor);
		ANTOMIC_ASSERT(json.contains("rotation"), "Missing sprite rotation");
		auto rot = json["rotation"].get<float>();
		ANTOMIC_ASSERT(json.contains("zorder"), "Missing sprite zorder");
		auto zorder = json["zorder"].get<int>();

		auto sprite = CreateRef<SpriteNode>(url);
		sprite->SetPosition(pos);
		sprite->SetSize(size);
		sprite->SetAnchor(anchor);
		sprite->SetRotation(rot);
		sprite->SetZOrder(zorder);

		return sprite;
	}

	void SpriteNode::CleanUp()
	{

		mSprite = nullptr;
		Node::CleanUp();
	}

} // namespace Antomic