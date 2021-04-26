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
#include "Renderer/Renderer.h"
#include "Renderer/Scene.h"
#include "Core/Cameras.h"
#include "Core/Application.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Drawable.h"
#include "Renderer/Scene.h"
#include "Renderer/RendererFrame.h"
#include "Renderer/RendererWorker.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Antomic
{
    QueueRef<RendererFrame> Renderer::sRenderQueue;
    Ref<RendererWorker> Renderer::sWorker = nullptr;
    Ref<RendererFrame> Renderer::sLastFrame = nullptr;
    Ref<Scene> Renderer::sScene = nullptr;
    std::mutex Renderer::sSceneMutex;
    std::mutex Renderer::sFrameMutex;
    std::thread Renderer::sThread;

    const Ref<Scene> &Renderer::GetCurrentScene()
    {
        std::lock_guard<std::mutex> lock(sSceneMutex);
        return sScene;
    }

    void Renderer::SetCurrentScene(const Ref<Scene> &scene)
    {
        std::lock_guard<std::mutex> lock(sSceneMutex);
        sScene = scene;
    }

    void Renderer::Submit(const Ref<RendererFrame> &frame, const Ref<Drawable> &drawable)
    {
        frame->QueueDrawable(drawable);
    }

    void Renderer::QueueFrame(const Ref<RendererFrame> &frame)
    {
        std::lock_guard<std::mutex> lock(sFrameMutex);
        sRenderQueue.push(frame);
    }

    const Ref<RendererFrame> Renderer::PopFrame()
    {
        std::lock_guard<std::mutex> lock(sFrameMutex);
        auto scene = sRenderQueue.back();
        sRenderQueue.pop();
        return scene;
    }

    void Renderer::RenderFrame(uint32_t width, uint32_t height)
    {
        if (sRenderQueue.empty())
        {
            return;
        }

        // Get the next frame to be drawn
        auto frame = PopFrame();

        RenderCommand::SetViewport(0, 0, width, height);
        RenderCommand::SetClearColor({0.5f, 0.1f, 0.8f, 1.0f});
        RenderCommand::Clear();

        auto scene = frame->GetScene();

        auto m_proj = scene->GetProjectionMatrix(width, height);
        auto m_view = scene->GetViewMatrix();
        auto m_projview = m_proj * m_view;

        while (!frame->Empty())
        {
            auto drawable = frame->PopDrawable();
            auto shader = drawable->GetShader();
            auto va = drawable->GetVertexArray();
            auto m_model = drawable->GetMatrix();

            shader->SetUniformValue("m_proj", m_proj);
            shader->SetUniformValue("m_view", m_view);
            shader->SetUniformValue("m_model", m_model);
            shader->SetUniformValue("m_projview", m_projview);

            shader->Bind();
            RenderCommand::DrawIndexed(va);
        }

        frame->SetEndTime(Platform::GetCurrentTick());
        sLastFrame = frame;
        Platform::SwapChain();
    }

    void Renderer::StartWorker()
    {
        ANTOMIC_ASSERT(!sWorker, "RenderWorker: Worker already running")
        sWorker = CreateRef<RendererWorker>();
        sThread = std::thread(&RendererWorker::Run, sWorker);
    }

    void Renderer::StopWorker()
    {
        ANTOMIC_ASSERT(sWorker, "RenderWorker: Worker not running")
        sWorker->Stop();
        sThread.join();
    }

    const uint32_t Renderer::GetLastFrameTime()
    {
        if (sLastFrame == nullptr)
        {
            return Platform::GetCurrentTick();
        }

        return sLastFrame->GetEndTime();
    }

} // namespace Anatomic