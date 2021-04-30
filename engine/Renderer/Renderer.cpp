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
#include "Graph/Scene.h"
#include "Renderer/Camera.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Drawable.h"
#include "Graph/Scene.h"
#include "Renderer/RendererFrame.h"
#include "Renderer/RendererWorker.h"
#include "Core/Application.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Antomic
{
    Renderer::Renderer(const RendererViewport &viewport)
    {
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
    }

    void Renderer::RenderFrame()
    {
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

        // Get Projection & View matrices
        auto m_proj = mProjectionMatrix;
        auto m_view = mScene->GetViewMatrix();
        auto m_projview = m_proj * m_view;

        // Ask scene to submit drawables to this frame
        mScene->SubmitDrawables(frame,m_view);

        // Start the rendering process
        RenderCommand::SetViewport(mViewport.Left, mViewport.Top, mViewport.Right, mViewport.Bottom);
        RenderCommand::SetClearColor(mViewport.Color);
        RenderCommand::Clear();

        while (!frame->Empty())
        {
            auto drawable = frame->PopDrawable();
            auto shader = drawable->GetShader();
            auto va = drawable->GetVertexArray();
            auto m_model = drawable->GetModelMatrix();

            for(auto bindable : drawable->GetBindables())
            {
                bindable->Bind();
            }

            shader->SetUniformValue("m_proj", m_proj);
            shader->SetUniformValue("m_view", m_view);
            shader->SetUniformValue("m_model", m_model);
            shader->SetUniformValue("m_projview", m_projview);

            shader->Bind();
            RenderCommand::DrawIndexed(va);
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