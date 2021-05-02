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
#include "Renderer/Sprite.h"
#include "Renderer/Texture.h"
#include "Renderer/RendererFrame.h"
#include "stb_image.h"
#include <glm/gtx/matrix_transform_2d.hpp>
#include "Profiling/Instrumentor.h"

namespace Antomic
{
    SpriteNode::SpriteNode(const std::string name)
    {
        {
            mSprite = CreateRef<Sprite>();

            // TODO: change the way we load resources
            // load and generate the texture
            int width, height, nrChannels;
            unsigned char *data = stbi_load(name.c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                Ref<Texture> texture = Texture::CreateTexture(width, height, data);
                mSprite->AddBindable(texture);
            }
            stbi_image_free(data);
        }
    }
}