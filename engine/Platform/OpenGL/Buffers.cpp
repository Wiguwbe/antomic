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
#include "Platform/OpenGL/Buffers.h"
#include "Platform/OpenGL/Shader.h"
#include "glad/glad.h"

namespace Antomic
{

    /* IndexBuffer Implementation */

    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *data, uint32_t size)
    {
        glCreateBuffers(1, &mRendererId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        mCount = size / sizeof(uint32_t);
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer()
    {
        glDeleteBuffers(1, &mRendererId);
    }

    void OpenGLIndexBuffer::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererId);
    }

    void OpenGLIndexBuffer::Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void OpenGLIndexBuffer::Upload(uint32_t *data, uint32_t size) const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    /* VertexBuffer Implementation */

    OpenGLVertexBuffer::OpenGLVertexBuffer(float *data, uint32_t size)
    {
        glCreateBuffers(1, &mRendererId);
        glBindBuffer(GL_ARRAY_BUFFER, mRendererId);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        glDeleteBuffers(1, &mRendererId);
    }

    void OpenGLVertexBuffer::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, mRendererId);
    }

    void OpenGLVertexBuffer::Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void OpenGLVertexBuffer::Upload(uint32_t *data, uint32_t size) const
    {
        glBindBuffer(GL_ARRAY_BUFFER, mRendererId);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void OpenGLVertexBuffer::SetLayout(const BufferLayout &layout)
    {
        mLayout = layout;
    }

}
