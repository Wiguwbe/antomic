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
#include "glm/glm.hpp"

namespace Antomic
{
    class RendererFrame
    {
    public:
        RendererFrame(const glm::mat4 &proj, const glm::mat4 &view, uint32_t width, uint32_t height)
            : mProj(proj), mView(view), mWidth(width), mHeight(height), mQueue() {}
        virtual ~RendererFrame() {}

        const glm::mat4 &GetProjection() const { return mProj; }
        const glm::mat4 &GetView() const { return mView; }
        const uint32_t &GetWidth() const { return mWidth; }
        const uint32_t &GetHeight() const { return mHeight; }
        void QueueDrawable(const Ref<Drawable> &drawable) { mQueue.push(drawable); }
        Ref<Drawable> PopDrawable();
        bool Empty() { return mQueue.empty(); }

    private:
        glm::mat4 mProj;
        glm::mat4 mView;
        uint32_t mWidth, mHeight;
        QueueRef<Drawable> mQueue;
    };


} // namespace Antomic