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
#include "Renderer/RendererWorker.h"
#include "Renderer/Scene.h"
#include "Renderer/Renderer.h"
#include "Renderer/RendererFrame.h"
#include "Platform/Platform.h"
#include "Core/Log.h"

#define MAX_FRAME_COUNT 3

namespace Antomic
{
    RendererWorker::RendererWorker()
        : mRunning(false)
    {
    }

    RendererWorker::~RendererWorker()
    {
    }

    void RendererWorker::Run()
    {
        if (mRunning)
        {
            return;
        }
        ANTOMIC_INFO("RendererWorker: Render worker started");
        mRunning = true;

        while (mRunning)
        {
            // Get the current scene from the renderer

            auto scene = Renderer::GetCurrentScene();
            if (Renderer::FrameCount() > MAX_FRAME_COUNT || scene == nullptr)
            {
                continue;
            }

            // We create a new frame
            auto frame = CreateRef<RendererFrame>(scene);

            // Get the time passed since last frame
            auto currentTime = Platform::GetCurrentTick();
            frame->SetStartTime(currentTime);

            // Update the current scene state
            scene->Update(currentTime - Renderer::GetLastFrameTime());

            // Submit the drawables for rendering
            scene->SubmitDrawables(frame);

            // TODO: Optmize render

            // Add this frame to the queue so it can be rendered
            Renderer::QueueFrame(frame);
        }

        ANTOMIC_INFO("RendererWorker: Render worker stopped");
    }

    void RendererWorker::Stop()
    {
        mRunning = false;
    }

} // namespace Antomic
