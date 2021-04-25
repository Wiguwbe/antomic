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
        mDirection = {0, 0, 0};
        mPosition = {0, 0, 3};
        mLookAt = {0, 0, 0};
    }

    void Game::LoadMainScene(const std::string &scene)
    {
        auto width = (float)GetWidth();
        auto height = (float)GetHeight();

        mCamera = Camera::CreatePerspective(glm::radians(45.0f), width/height , 0.1f, 100.0f);

        Camera::UpdateCamera(
            mCamera,
            mPosition,         // Camera is at (4,3,3), in World Space
            mLookAt,           // and looks at the origin
            glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
        );

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

            Ref<Scene> scene = CreateRef<Scene>(mCamera);
            auto drawable = CreateRef<Drawable>(vertexArray, shader);
            scene->AddDrawable(drawable);
            SetScene(scene);
        }
    }

    bool Game::OnWindowResize(WindowResizeEvent &event)
    {
        Camera::UpdateCamera(mCamera, glm::radians(45.0f), (float)event.GetWidth() / (float)event.GetHeight(), 0.1f, 100.0f);
        return true;
    }

    bool Game::OnKeyPressed(KeyPressedEvent &event)
    {

        switch (event.GetKeyCode())
        {
        case Key::Enum::KeyW:
            mDirection.y = 1;
            break;
        case Key::Enum::KeyS:
            mDirection.y = -1;
            break;
        case Key::Enum::KeyA:
            mDirection.x = -1;
            break;
        case Key::Enum::KeyD:
            mDirection.x = 1;
            break;
        default:
            break;
        }

        return true;
    }

    bool Game::OnKeyReleased(KeyReleasedEvent &event)
    {
        switch (event.GetKeyCode())
        {
        case Key::Enum::KeyW:
        case Key::Enum::KeyS:
            mDirection.y = 0;
            break;
        case Key::Enum::KeyA:
        case Key::Enum::KeyD:
            mDirection.x = 0;
            break;
        default:
            break;
        }

        return true;
    }

} // namespace Antomic

// void Game::Render()
// {
//     glm::mat4 m_proj = glm::perspective();

//     // Camera matrix
//     glm::mat4 m_view = glm::lookAt(
//         mCamera,            // Camera is at (4,3,3), in World Space
//         mLookAt, // and looks at the origin
//         glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
//     );

//     glm::mat4 m_model = glm::mat4(1.0f);
//     glm::mat4 m_mvp = m_proj * m_view * m_model;

//     RenderCommand::SetViewport(0, 0, GetWidth(), GetHeight());
//     RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
//     RenderCommand::Clear();
//     mShader->SetUniformValue("m_proj", m_proj);
//     mShader->SetUniformValue("m_view", m_view);
//     mShader->SetUniformValue("m_model", m_model);
//     mShader->SetUniformValue("m_mvp", m_mvp);
//     mShader->Bind();
//     RenderCommand::DrawIndexed(mVertexArray);
// }

// void Game::Update(const uint32_t &timestep)
// {
//     mCamera += (((float)timestep / 1000.f) * mDirection);
//     mLookAt += (((float)timestep / 1000.f) * mDirection);
// }
