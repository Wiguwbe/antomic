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
#include "Renderer/Buffers.h"

namespace Antomic {

    class VertexArray
    {
    public:
        ~VertexArray() = default;

    public:

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual void AddVertexBuffer(const Ref<VertexBuffer> &buffer) = 0;
        virtual void SetIndexBuffer(const Ref<IndexBuffer> &buffer) = 0;
        virtual const std::vector<Ref<VertexBuffer>> &GetVertexBuffers() const = 0;
        virtual const Ref<IndexBuffer> &GetIndexBuffer() const = 0;

    public:
        static Ref<VertexArray> Create();
    };
}