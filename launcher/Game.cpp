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

namespace Antomic
{
    Game::Game() : Application("Game")
    {
    }

    void Game::LoadMainScene(const std::string &scene)
    {
        {
            auto vertexArray = VertexArray::Create();
            auto shader = Shader::CreateFromFile("media/shaders/opengl/vs_demo_1.glsl", "media/shaders/opengl/fs_demo_1.glsl");

            float vertices[3 * 3] = {
                -0.5f,
                -0.5f,
                0.0f,
                0.5f,
                -0.5f,
                0.0f,
                0.0f,
                0.5f,
                0.0f,
            };
            uint32_t indices[3] = {0, 1, 2};

            BufferLayout layout = {
                {ShaderDataType::Float3, "a_Position"}};

            Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
            vertexBuffer->SetLayout(layout);
            Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, sizeof(indices));
            vertexArray->AddVertexBuffer(vertexBuffer);
            vertexArray->SetIndexBuffer(indexBuffer);

            auto scene = CreateRef<Scene>();
            auto triangle = CreateRef<Mesh>(vertexArray, shader);
            scene->AddDrawable(triangle);
            SetScene(scene);
        }
    }
} // namespace Antomic
