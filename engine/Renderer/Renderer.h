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
    class Renderer
    {
    public:
        static void SubmitScene(const Ref<Scene> &scene);
        static void Submit(const Ref<RendererFrame> &frame, const Ref<Drawable> &drawable);
        static void RenderFrame(uint32_t width, uint32_t height, const glm::mat4 &proj);
        static void QueueFrame(const Ref<RendererFrame> &frame);
        static void StartWorker();
        static void StopWorker();
        static const Ref<Scene> &GetCurrentScene();
        static void SetCurrentScene(const Ref<Scene> &scene);
        inline static const Ref<RendererFrame> GetLastFrame() { return sLastFrame; }
        inline static uint32_t FrameCount() { return sRenderQueue.size(); }
        static const uint32_t GetLastFrameTime();

    private:
        static const Ref<RendererFrame> PopFrame();

    private:
        static QueueRef<RendererFrame> sRenderQueue;
        static Ref<RendererWorker> sWorker;
        static Ref<RendererFrame> sLastFrame;
        static Ref<Scene> sScene;
        static std::mutex sFrameMutex, sSceneMutex;
        static std::thread sThread;
    };
} // namespace Antomic