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
#include "Renderer/Buffers.h"
#include "Core/Log.h"
#include "Platform/NullRenderer/Buffers.h"
#include "Platform/Platform.h"
#include "Platform/RenderAPI.h"
#ifdef ANTOMIC_GL_RENDERER
#	include "Platform/OpenGL/Buffers.h"
#endif

namespace Antomic
{
	/*************************************************************
     * Bufer Layout Implementation
     *************************************************************/

	BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements)
		: mElements(elements)
	{
		Update();
	};

	void BufferLayout::Update()
	{

		mStride = 0;
		for(auto& element : mElements)
		{
			element.Offset = mStride;
			mStride += element.Size;
		}
	}

	/*************************************************************
     * UniformBufferLayout implementation
     *************************************************************/

	UniformBufferLayout::UniformBufferLayout(
		const std::initializer_list<UniformBufferElement>& elements)
		: mElements(elements)
	{
		Update();
	};

	void UniformBufferLayout::Update()
	{
		// Get the aligned offset
		auto align = [](int offset, int base_alignment) -> int {
			auto r = offset % base_alignment;
			if(r == 0)
				return offset;
			return (offset - r) + base_alignment;
		};

		// According to Sub-section 2.15.3.1.2 - Standard Uniform Block Layout
		// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
		mStride = 0;
		for(auto& element : mElements)
		{
			element.Offset = align(mStride, element.BaseAlignment);
			mStride = element.Offset +
					  (element.Count * align(element.Size,
											 element.Count == 1
												 ? element.BaseAlignment
												 : ShaderDataTypeAlignment(ShaderDataType::Vec4)));
		}
	}

	/*************************************************************
     * IndexBuffer Implementation
     *************************************************************/

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* data, uint32_t size)
	{
		switch(Platform::GetRenderAPIDialect())
		{
#ifdef ANTOMIC_GL_RENDERER
		case RenderAPIDialect::OPENGL:
			return CreateRef<OpenGLIndexBuffer>(data, size);
#endif
		default:
			return CreateRef<NullIndexBuffer>(data, size);
		}
	}

	/*************************************************************
     * VertexBuffer Implementation
     *************************************************************/

	Ref<VertexBuffer> VertexBuffer::Create(float* data, uint32_t size)
	{
		switch(Platform::GetRenderAPIDialect())
		{
#ifdef ANTOMIC_GL_RENDERER
		case RenderAPIDialect::OPENGL:
			return CreateRef<OpenGLVertexBuffer>(data, size);
#endif
		default:
			return CreateRef<NullVertexBuffer>(data, size);
		}
	}

	/*************************************************************
     * UniformBuffer Implementation
     *************************************************************/

	Ref<UniformBuffer> UniformBuffer::Create(const UniformBufferLayout& layout, uint32_t binding)
	{
		switch(Platform::GetRenderAPIDialect())
		{
#ifdef ANTOMIC_GL_RENDERER
		case RenderAPIDialect::OPENGL:
			return CreateRef<OpenGLUniformBuffer>(layout, binding);
#endif
		default:
			return CreateRef<NullUniformBuffer>(layout, binding);
		}
	}

	const UniformBufferElement& UniformBufferLayout::GetElement(const std::string& name)
	{
		auto is_element = [&name](const UniformBufferElement& element) -> bool {
			return element.Name.compare(name) == 0;
		};

		auto it = std::find_if(mElements.begin(), mElements.end(), is_element);
		ANTOMIC_ASSERT(it != mElements.end(), "UniformBufferLayout: Element does not exists!")
		return *it;
	}
} // namespace Antomic