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
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"

namespace Antomic
{
    Game::Game() : Application("Game")
    {
    }

    void Game::LoadMainScene(const std::string &scene)
    {
        {
            auto vertexArray = VertexArray::Create();
            auto material = CreateRef<BasicMaterial>();

            float vertices[] = {
                // positions          // colors           // texture coords
                0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
                0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
                -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
            };

            uint32_t indices[6] = {0, 1, 3, 1, 2, 3};

            BufferLayout layout = {
                {ShaderDataType::Vec3, "aPos"},
                {ShaderDataType::Vec3, "aColor"},
                {ShaderDataType::Vec2, "aTexCoord"}};

            Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
            vertexBuffer->SetLayout(layout);
            Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, sizeof(indices));
            vertexArray->AddVertexBuffer(vertexBuffer);
            vertexArray->SetIndexBuffer(indexBuffer);

            auto triangle = CreateRef<Drawable>(vertexArray, material);

            // load and generate the texture
            int width, height, nrChannels;
            unsigned char *data = stbi_load("media/textures/container.jpg", &width, &height, &nrChannels, 0);
            if (data)
            {
                Ref<Texture> texture = Texture::CreateTexture(width, height, data);
                triangle->AddBindable(texture);
            }
            stbi_image_free(data);

            auto scene = CreateRef<Scene>();
            scene->AddDrawable(triangle);
            SetScene(scene);
        }
    }
} // namespace Antomic
