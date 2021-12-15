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
#include "Platform/OpenGL/VertexArray.h"
#include "Platform/OpenGL/Shader.h"
#include "glad/glad.h"

namespace Antomic
{
	OpenGLVertexArray::OpenGLVertexArray() { glCreateVertexArrays(1, &mRendererId); }

	OpenGLVertexArray::~OpenGLVertexArray() { glDeleteVertexArrays(1, &mRendererId); }

	void OpenGLVertexArray::Bind() const { glBindVertexArray(mRendererId); }

	void OpenGLVertexArray::Unbind() const { glBindVertexArray(0); }

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& buffer)
	{
		glBindVertexArray(mRendererId);
		buffer->Bind();
		auto const& layout = buffer->Layout();
		uint32_t index = 0;
		for(auto& element : layout.Elements())
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
								  ShaderDataTypeGLSize(element.Type),
								  ShaderDataTypeGLEnum(element.Type),
								  element.Normalized ? GL_TRUE : GL_FALSE,
								  layout.Stride(),
								  (const GLvoid*)((intptr_t)element.Offset));
			index++;
		}
		mVertextBuffers.push_back(buffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& buffer)
	{
		glBindVertexArray(mRendererId);
		buffer->Bind();
		mIndexBuffer = buffer;
	}

} // namespace Antomic