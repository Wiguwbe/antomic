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
#include "Renderer/Render2d.h"
#include "Renderer/Buffers.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Shader.h"
#include "Renderer/Sprite.h"
#include "Renderer/VertexArray.h"

namespace Antomic
{
	static Ref<VertexArray> sVertexArray = nullptr;
	static Ref<Shader> sShader = nullptr;

	void Render2d::Init()
	{
		sVertexArray = VertexArray::Create();
		sShader = Shader::CreateFromFile("assets/shaders/2d/vs_sprite.glsl",
										 "assets/shaders/2d/fs_sprite.glsl");

		// Vertices for our quad
		float vertices[] = {// pos      // tex
							0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

							0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};

		uint32_t indices[6] = {0, 1, 2, 3, 4, 5};

		// Set the buffer layout for the shader
		BufferLayout layout = {{ShaderDataType::Vec2, "m_pos"}, {ShaderDataType::Vec2, "m_tex"}};

		// Create a vertex buffer for the quad
		auto vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		vertexBuffer->SetLayout(layout);
		sVertexArray->AddVertexBuffer(vertexBuffer);

		// Index buffer for the quad
		auto indexBuffer = IndexBuffer::Create(indices, sizeof(indices));
		sVertexArray->SetIndexBuffer(indexBuffer);
	}

	void Render2d::DrawSprite(const Ref<Sprite>& sprite)
	{
		sShader->SetUniformValue("m_model", sprite->GetModelMatrix());
		sShader->SetUniformValue("m_color", sprite->GetSpriteColor());
		sShader->Bind();
		for(auto bindable : sprite->GetBindables())
		{
			bindable->Bind();
		}
		RenderCommand::DrawIndexed(sVertexArray);
	}

	void Render2d::DrawSprite(const Sprite& sprite)
	{
		sShader->SetUniformValue("m_model", sprite.GetModelMatrix());
		sShader->SetUniformValue("m_color", sprite.GetSpriteColor());
		sShader->Bind();
		for(auto bindable : sprite.GetBindables())
		{
			bindable->Bind();
		}
		RenderCommand::DrawIndexed(sVertexArray);
	}

	void Render2d::Shutdown()
	{
		// CleanUp our vertex array and shader
		sVertexArray = nullptr;
		sShader = nullptr;
	}
} // namespace Antomic