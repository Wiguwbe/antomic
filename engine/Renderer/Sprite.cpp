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
#include "Renderer/Sprite.h"
#include "Renderer/Bindable.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/RenderCommand.h"

namespace Antomic
{
    Sprite::Sprite()
    {
        {
            mVertexArray = VertexArray::Create();
            mShader = Shader::CreateFromFile("assets/shaders/2d/vs_sprite.glsl", "assets/shaders/2d/fs_sprite.glsl");

            // Vertices for our quad
            float vertices[] = {
                // pos      // tex
                0.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,

                0.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 0.0f};

            uint32_t indices[6] = {0, 1, 2, 3, 4, 5};

            // Set the buffer layout for the shader
            BufferLayout layout = {
                {ShaderDataType::Vec2, "m_pos"},
                {ShaderDataType::Vec2, "m_tex"}};

            // Create a vertex buffer for the quad
            auto vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
            vertexBuffer->SetLayout(layout);
            mVertexArray->AddVertexBuffer(vertexBuffer);

            // Index buffer for the quad
            auto indexBuffer = IndexBuffer::Create(indices, sizeof(indices));
            mVertexArray->SetIndexBuffer(indexBuffer);
        }
    }

    void Sprite::Draw()
    {
        mShader->SetUniformValue("m_model", GetModelMatrix());
        mShader->SetUniformValue("m_color", mSpriteColor);
        mShader->Bind();
        for (auto bindable : GetBindables())
        {
            bindable->Bind();
        }
        RenderCommand::DrawIndexed(mVertexArray);
    }

} // namespace Antomic
