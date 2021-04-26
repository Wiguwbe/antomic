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
#include "Core/Log.h"

namespace Antomic
{
    QueueRef<Scene> RendererWorker::sSceneQueue;
    Ref<RendererWorker> RendererWorker::sWorker = nullptr;
    std::mutex RendererWorker::sMutex;
    std::thread RendererWorker::sThread;

    RendererWorker::RendererWorker()
        : mRunning(false)
    {
    }

    RendererWorker::~RendererWorker()
    {
    }

    void RendererWorker::StartWorker()
    {
        ANTOMIC_ASSERT(!sWorker, "RenderWorker: Worker already running")
        sWorker = CreateRef<RendererWorker>();
        sThread = std::thread(&RendererWorker::Run, sWorker);
    }

    void RendererWorker::StopWorker()
    {
        ANTOMIC_ASSERT(sWorker, "RenderWorker: Worker not running")
        sWorker->mRunning = false;
        sThread.join();
    }

    void RendererWorker::SubmitScene(const Ref<Scene> &scene)
    {
        std::lock_guard<std::mutex> lock(sMutex);
        sSceneQueue.push(scene);
    }

    const Ref<Scene> RendererWorker::PopScene()
    {
        std::lock_guard<std::mutex> lock(sMutex);
        auto scene = sSceneQueue.back();
        sSceneQueue.pop();
        return scene;
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
            if (sSceneQueue.empty())
            {
                continue;
            }

            auto scene = PopScene();
            auto frame = CreateRef<RendererFrame>(scene->GetProjection(), scene->GetView());
            scene->SubmitDrawables(frame);
            // TODO: Optmize render
            Renderer::QueueFrame(frame);
        }

        ANTOMIC_INFO("RendererWorker: Render worker stopped");
    }

} // namespace Antomic
