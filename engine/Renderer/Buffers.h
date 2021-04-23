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
#include "Renderer/Shader.h"

namespace Antomic
{
    struct BufferElement {
        std::string Name;
        ShaderDataType Type;
        uint32_t Size;
        uint32_t Offset;
        bool Normalized;

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false) 
            : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {}
    };

    class BufferLayout {
        public:
            BufferLayout(const std::initializer_list<BufferElement>& elements);
            BufferLayout() {}
            ~BufferLayout() {};

            inline const std::vector<BufferElement>& Elements() const { return mElements; }
            inline uint32_t Stride() const { return mStride; }
        private:
            void Update();

        private:
            std::vector<BufferElement> mElements;
            uint32_t mStride = 0;
    };

    class IndexBuffer
    {        
    public:
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual void Upload(uint32_t* data, uint32_t size) const = 0;
	public:
		static Ref<IndexBuffer> Create(uint32_t* data, uint32_t size);
    };

    class VertexBuffer
    {
        
    public:
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual void Upload(uint32_t* data, uint32_t size) const = 0;
        virtual const BufferLayout &Layout() const = 0;
        virtual void SetLayout(const BufferLayout &layout) = 0;
	public:
		static Ref<VertexBuffer> Create(float* data, uint32_t size);
    };



} // namespace Antomic
