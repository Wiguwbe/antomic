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
#include <glm/gtc/type_ptr.hpp>

namespace Antomic
{

    /*************************************************************
     * OpenGLIndexBuffer Implementation
     *************************************************************/

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

    /*************************************************************
     * OpenGLVertexBuffer Implementation
     *************************************************************/

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

    /*************************************************************
     * OpenGLVertexBuffer Implementation
     *************************************************************/

    OpenGLUniformBuffer::OpenGLUniformBuffer(const UniformBufferLayout &layout, uint32_t binding)
    {
        mLayout = layout;
        glCreateBuffers(1, &mRendererId);
        glNamedBufferData(mRendererId, mLayout.Stride(), nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, mRendererId);
    }

    OpenGLUniformBuffer::~OpenGLUniformBuffer()
    {
        glDeleteBuffers(1, &mRendererId);
    }

    void OpenGLUniformBuffer::SetValue(const std::string &name, const glm::mat4 &data)
    {
        auto element = mLayout.GetElement(name);
        glNamedBufferSubData(mRendererId, element.Offset, sizeof(glm::mat4), glm::value_ptr(data));
    }

    void OpenGLUniformBuffer::SetValue(const std::string &name, const glm::mat3 &data)
    {
        auto element = mLayout.GetElement(name);
        glNamedBufferSubData(mRendererId, element.Offset, sizeof(glm::mat4), glm::value_ptr(glm::mat4(data)));
    }

    void OpenGLUniformBuffer::SetValue(const std::string &name, const glm::vec4 &data)
    {
        auto element = mLayout.GetElement(name);
        glNamedBufferSubData(mRendererId, element.Offset, sizeof(glm::vec4), glm::value_ptr(data));
    }

    void OpenGLUniformBuffer::SetValue(const std::string &name, const glm::vec3 &data)
    {
        auto element = mLayout.GetElement(name);
        glNamedBufferSubData(mRendererId, element.Offset, sizeof(glm::vec4), glm::value_ptr(glm::vec4(data, 0.0f)));
    }

    void OpenGLUniformBuffer::SetValue(const std::string &name, const glm::vec2 &data)
    {
        auto element = mLayout.GetElement(name);
        glNamedBufferSubData(mRendererId, element.Offset, sizeof(glm::vec2), glm::value_ptr(data));
    }

    void OpenGLUniformBuffer::SetValue(const std::string &name, float data)
    {
        auto element = mLayout.GetElement(name);
        glNamedBufferSubData(mRendererId, element.Offset, sizeof(float), &data);
    }

    void OpenGLUniformBuffer::SetValue(const std::string &name, uint32_t data)
    {
        auto element = mLayout.GetElement(name);
        glNamedBufferSubData(mRendererId, element.Offset, sizeof(uint32_t), &data);
    }

    void OpenGLUniformBuffer::SetValue(const std::string &name, int data)
    {
        auto element = mLayout.GetElement(name);
        glNamedBufferSubData(mRendererId, element.Offset, sizeof(int), &data);
    }

    void OpenGLUniformBuffer::SetValue(const std::string &name, bool data)
    {
        auto element = mLayout.GetElement(name);
        glNamedBufferSubData(mRendererId, element.Offset, sizeof(bool), &data);
    }
}
