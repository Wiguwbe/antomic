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

namespace Antomic
{
    class RendererFrame
    {
    public:
        RendererFrame(Ref<Scene> scene) : mDrawableQueue(), mScene(scene) {}
        virtual ~RendererFrame() {}

        const Ref<Scene> &GetScene() const { return mScene; }

        inline const uint32_t &GetStartTime() const { return mStartTime; }
        inline void SetStartTime(uint32_t time) { mStartTime = time; }
        inline const uint32_t &GetEndTime() const { return mEndTime; }
        inline void SetEndTime(uint32_t time) { mEndTime = time; }

        void QueueDrawable(const Ref<Drawable> &drawable) { mDrawableQueue.push(drawable); }
        Ref<Drawable> PopDrawable();

        bool Empty() { return mDrawableQueue.empty(); }

    private:
        uint32_t mStartTime, mEndTime;
        QueueRef<Drawable> mDrawableQueue;
        Ref<Scene> mScene;
    };

} // namespace Antomic