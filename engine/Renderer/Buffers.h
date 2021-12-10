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
#include "Core/Base.h"
#include "Renderer/Bindable.h"
#include "Renderer/Shader.h"

namespace Antomic
{
	/*************************************************************
     * BufferElement Implementation
     *************************************************************/

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name)
			, Type(type)
			, Size(ShaderDataTypeSize(type))
			, Offset(0)
			, Normalized(normalized)
		{ }
	};

	/*************************************************************
     * UniformBufferElement Implementation
     *************************************************************/

	struct UniformBufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint8_t Size;
		uint8_t BaseAlignment;
		uint8_t Count;
		uint32_t Offset;

		UniformBufferElement(ShaderDataType type, const std::string& name)
			: Name(name)
			, Type(type)
			, Size(ShaderDataTypeSize(type))
			, BaseAlignment(ShaderDataTypeAlignment(type))
			, Count(1)
			, Offset(0)
		{ }

		UniformBufferElement(ShaderDataType type, uint8_t count, const std::string& name)
			: Name(name)
			, Type(type)
			, Size(ShaderDataTypeSize(type))
			, BaseAlignment(ShaderDataTypeAlignment(type))
			, Count(count)
			, Offset(0)
		{ }
	};

	/*************************************************************
     * BufferLayout Implementation
     *************************************************************/

	class BufferLayout
	{
	public:
		BufferLayout(const std::initializer_list<BufferElement>& elements);
		BufferLayout() { }
		~BufferLayout(){};

	public:
		inline const std::vector<BufferElement>& Elements() const
		{
			return mElements;
		}
		inline uint32_t Stride() const
		{
			return mStride;
		}

	private:
		void Update();

	private:
		std::vector<BufferElement> mElements;
		uint32_t mStride = 0;
	};

	/*************************************************************
     * UniformBufferLayout Implementation
     *************************************************************/

	class UniformBufferLayout
	{
	public:
		UniformBufferLayout(const std::initializer_list<UniformBufferElement>& elements);
		UniformBufferLayout() { }
		~UniformBufferLayout(){};

	public:
		inline const std::vector<UniformBufferElement>& Elements() const
		{
			return mElements;
		}
		inline uint32_t Stride() const
		{
			return mStride;
		}
		const UniformBufferElement& GetElement(const std::string& name);

	private:
		void Update();

	private:
		std::vector<UniformBufferElement> mElements;
		uint32_t mStride = 0;
	};

	/*************************************************************
     * IndexBuffer Implementation
     *************************************************************/

	class IndexBuffer : public Bindable
	{
	public:
		virtual ~IndexBuffer() = default;

	public:
		virtual void Upload(uint32_t* data, uint32_t size) const = 0;
		virtual uint32_t Count() const = 0;

	public:
		static Ref<IndexBuffer> Create(uint32_t* data, uint32_t size);
	};

	/*************************************************************
     * VertexBuffer Implementation
     *************************************************************/

	class VertexBuffer : public Bindable
	{
	public:
		virtual ~VertexBuffer() = default;

	public:
		virtual void Upload(uint32_t* data, uint32_t size) const = 0;
		virtual const BufferLayout& Layout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

	public:
		static Ref<VertexBuffer> Create(float* data, uint32_t size);
	};

	/*************************************************************
     * UniformBuffer Implementation
     *************************************************************/

	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;

	public:
		virtual void SetValue(const std::string& name, const glm::mat4& data) = 0;
		virtual void SetValue(const std::string& name, const glm::mat3& data) = 0;
		virtual void SetValue(const std::string& name, const glm::vec4& data) = 0;
		virtual void SetValue(const std::string& name, const glm::vec3& data) = 0;
		virtual void SetValue(const std::string& name, const glm::vec2& data) = 0;
		virtual void SetValue(const std::string& name, float data) = 0;
		virtual void SetValue(const std::string& name, uint32_t data) = 0;
		virtual void SetValue(const std::string& name, int data) = 0;
		virtual void SetValue(const std::string& name, bool data) = 0;
		virtual const UniformBufferLayout& Layout() const = 0;

	public:
		static Ref<UniformBuffer> Create(const UniformBufferLayout& layout, uint32_t binding);
	};

} // namespace Antomic
