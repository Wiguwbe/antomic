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

namespace Launcher
{

    Game::Game() : Antomic::Application("Game")
    {

        mVertexArray = Antomic::VertexArray::Create();
        mShader = Antomic::Shader::CreateFromFile("media/shaders/opengl/vs_demo_1.glsl", "media/shaders/opengl/fs_demo_1.glsl");

        {
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

            Antomic::BufferLayout layout = {
                {Antomic::ShaderDataType::Float3, "a_Position"}};

            Antomic::Ref<Antomic::VertexBuffer> vertexBuffer = Antomic::VertexBuffer::Create(vertices, sizeof(vertices));
            vertexBuffer->SetLayout(layout);
            Antomic::Ref<Antomic::IndexBuffer> indexBuffer = Antomic::IndexBuffer::Create(indices, sizeof(indices));
            mVertexArray->AddVertexBuffer(vertexBuffer);
            mVertexArray->SetIndexBuffer(indexBuffer);
        }
    }

    void Game::Render()
    {
        Antomic::Application::Render();
        Antomic::RenderCommand::Clear({0.1f, 0.1f, 0.1f, 1.0f});
        mShader->Bind();
        Antomic::RenderCommand::DrawIndexed(mVertexArray);
    }

} // namespace Launcher