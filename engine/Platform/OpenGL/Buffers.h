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
#pragma once
#include "Renderer/Buffers.h"
#include "glad/glad.h"

namespace Antomic
{
	/*************************************************************
     * OpenGLIndexBuffer Implementation
     *************************************************************/

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* data, uint32_t size);
		virtual ~OpenGLIndexBuffer() override;

	public:
		// Bind/Unbind commands
		virtual void Bind() const override;
		virtual void Unbind() const override;

		// IndexBuffer commands
		virtual void Upload(uint32_t* data, uint32_t size) const override;
		virtual uint32_t Count() const override
		{
			return mCount;
		};

	private:
		GLuint mRendererId;
		uint32_t mCount;
	};

	/*************************************************************
     * OpenGLVertexBuffer Implementation
     *************************************************************/

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* data, uint32_t size);
		virtual ~OpenGLVertexBuffer() override;

	public:
		// Bind/Unbind commands
		virtual void Bind() const override;
		virtual void Unbind() const override;

		// VertexBuffer commands
		virtual void Upload(uint32_t* data, uint32_t size) const override;
		virtual const BufferLayout& Layout() const
		{
			return mLayout;
		};
		virtual void SetLayout(const BufferLayout& layout) override;

	private:
		GLuint mRendererId;
		BufferLayout mLayout;
	};

	/*************************************************************
     * OPenGLUniformBuffer Implementation
     *************************************************************/

	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(const UniformBufferLayout& layout, uint32_t binding);
		virtual ~OpenGLUniformBuffer() override;

	public:
		// UniformBuffer commands
		virtual void SetValue(const std::string& name, const glm::mat4& data) override;
		virtual void SetValue(const std::string& name, const glm::mat3& data) override;
		virtual void SetValue(const std::string& name, const glm::vec4& data) override;
		virtual void SetValue(const std::string& name, const glm::vec3& data) override;
		virtual void SetValue(const std::string& name, const glm::vec2& data) override;
		virtual void SetValue(const std::string& name, float data) override;
		virtual void SetValue(const std::string& name, uint32_t data) override;
		virtual void SetValue(const std::string& name, int data) override;
		virtual void SetValue(const std::string& name, bool data) override;
		virtual const UniformBufferLayout& Layout() const override
		{
			return mLayout;
		}

	private:
		GLuint mRendererId;
		UniformBufferLayout mLayout;
	};

} // namespace Antomic
