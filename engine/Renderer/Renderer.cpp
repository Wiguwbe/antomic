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
#include "Core/Application.h"
#include "Renderer/Camera.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Drawable.h"
#include "Renderer/RendererFrame.h"
#include "Renderer/RendererWorker.h"
#include "Renderer/Buffers.h"
#include "Graph/Scene.h"
#include "Profiling/Instrumentor.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Antomic
{
    Renderer::Renderer(const RendererViewport &viewport)
    {
        {
            UniformBufferLayout cameraBufferLayout = {
                {ShaderDataType::Mat4, "m_proj"},
                {ShaderDataType::Mat4, "m_view"},
                {ShaderDataType::Mat4, "m_projview"}};

            mCameraBuffer = UniformBuffer::Create(cameraBufferLayout, 0);
        }

        SetViewport(viewport);
    }

    const Ref<Scene> &Renderer::GetCurrentScene()
    {
        return mScene;
    }

    void Renderer::SetCurrentScene(const Ref<Scene> &scene)
    {
        mScene = scene;
        ANTOMIC_ASSERT(mScene->GetActiveCamera(), "Renderer: Scene without active camera!")
        mProjectionMatrix = mScene->GetActiveCamera()->GetProjectionMatrix(mViewport);
        mCameraBuffer->SetValue("m_proj", mProjectionMatrix);
    }

    void Renderer::SetViewport(const RendererViewport &viewport)
    {
        mViewport = viewport;
        if (mScene == nullptr)
        {
            mProjectionMatrix = glm::mat4(1.0f);
            return;
        }
        ANTOMIC_ASSERT(mScene->GetActiveCamera(), "Renderer: Scene without active camera!")
        mProjectionMatrix = mScene->GetActiveCamera()->GetProjectionMatrix(mViewport);
        mCameraBuffer->SetValue("m_proj", mProjectionMatrix);
    }

    void Renderer::RenderFrame()
    {
        ANTOMIC_PROFILE_FUNCTION();

        if (mScene == nullptr)
        {
            return;
        }

        // Create a new frame
        auto frame = CreateRef<RendererFrame>();

        // Get the time passed since last frame
        auto currentTime = Platform::GetCurrentTick();

        // Update the current scene state
        // Get the time passed since last frame
        auto timestep = currentTime - Renderer::GetLastFrameTime();
        mLastFrameTime = currentTime;
        mScene->Update(timestep);

        // Get View matrices
        auto m_view = mScene->GetViewMatrix();
        auto m_projview = mProjectionMatrix * m_view;

        // Ask scene to submit drawables to this frame
        mScene->SubmitDrawables(frame, m_view);

        // Start the rendering process
        RenderCommand::SetViewport(mViewport.Left, mViewport.Top, mViewport.Right, mViewport.Bottom);
        RenderCommand::SetClearColor(mViewport.Color);
        RenderCommand::Clear();

        mCameraBuffer->SetValue("m_view", m_view);
        mCameraBuffer->SetValue("m_projview", m_projview);

        while (!frame->Empty())
        {
            auto drawable = frame->PopDrawable();
            drawable->Draw();
        }

        mLastFrame = frame;
        Platform::SwapBuffer();
    }

    const uint32_t Renderer::GetLastFrameTime()
    {
        if (mLastFrame == nullptr)
        {
            return Platform::GetCurrentTick();
        }

        return mLastFrameTime;
    }

} // namespace Anatomic