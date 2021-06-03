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
#include "Renderer/Render2d.h"
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
                {ShaderDataType::Mat4, "m_projview"},
                {ShaderDataType::Mat4, "m_ortho"}};

            mCameraBuffer = UniformBuffer::Create(cameraBufferLayout, 0);
        }

        Render2d::Init();
        SetViewport(viewport);
    }

    Renderer::~Renderer()
    {
        Render2d::Shutdown();
    }

    const Ref<Scene> &Renderer::GetCurrentScene()
    {
        return mScene;
    }

    void Renderer::SetCurrentScene(const Ref<Scene> &scene)
    {
        mScene = scene;
        UpdateProjectionMatrix();
    }

    void Renderer::SetViewport(const RendererViewport &viewport)
    {
        mViewport = viewport;
        UpdateProjectionMatrix();
    }

    void Renderer::RenderFrame()
    {
        ANTOMIC_PROFILE_FUNCTION("Renderer");

        if (mScene == nullptr)
        {
            return;
        }

        // Get the time passed since last frame
        auto currentTime = Platform::GetCurrentTick();

        // Update the current scene state
        // Get the time passed since last frame
        auto timestep = currentTime - Renderer::GetLastFrameTime();
        mLastFrameTime = currentTime;
        mScene->Update((uint32_t) timestep);

        // Get View matrices
        auto viewMatrix = mScene->GetViewMatrix();
        auto projView = mProjectionMatrix * viewMatrix;

        // Update the values on our camera uniform
        mCameraBuffer->SetValue("m_view", viewMatrix);
        mCameraBuffer->SetValue("m_projview", projView);

        // Create a new frame
        auto frame = CreateRef<RendererFrame>(mViewport, viewMatrix);

        // Ask scene to submit drawables to this frame
        mScene->SubmitDrawables(frame);

        // Start the rendering process
        frame->Draw();
        mLastFrame = frame;
        Platform::SwapBuffer();
    }

    const uint64_t Renderer::GetLastFrameTime()
    {
        if (mLastFrame == nullptr)
        {
            return Platform::GetCurrentTick();
        }

        return mLastFrameTime;
    }

    void Renderer::UpdateProjectionMatrix()
    {
        if (mScene == nullptr)
        {
            mProjectionMatrix = glm::mat4(1.0f);
            return;
        }
        auto active = mScene->GetActiveCamera();
        ANTOMIC_ASSERT(active != nullptr, "Renderer: Scene without active camera!")
        mProjectionMatrix = active->GetProjectionMatrix(mViewport);
        mCameraBuffer->SetValue("m_proj", mProjectionMatrix);
        switch (active->GetType())
        {
        case CameraType::ORTOGRAPHIC:
            mCameraBuffer->SetValue("m_ortho", mProjectionMatrix);
            return;
        default:
            mCameraBuffer->SetValue("m_ortho", OrthographicCamera::ProjectionMatrix(mViewport));
            return;
        }
    }
} // namespace Anatomic