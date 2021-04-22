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

namespace Antomic
{
    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer(uint32_t *data, uint32_t size);
        ~OpenGLIndexBuffer();
        virtual void Bind() const;
        virtual void Unbind() const;
        virtual void Upload(uint32_t* data, uint32_t size) const;

    private:
        uint32_t mRendererId;
    };

    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer(float *data, uint32_t size);
        ~OpenGLVertexBuffer();
        virtual void Bind() const;
        virtual void Unbind() const;
        virtual void Upload(uint32_t* data, uint32_t size) const;

    private:
        uint32_t mRendererId;
    };

} // namespace Antomic
