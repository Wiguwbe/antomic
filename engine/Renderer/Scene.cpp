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
#include "Renderer/Scene.h"
#include "Renderer/Camera.h"
#include "Renderer/RendererWorker.h"
#include "Renderer/RendererFrame.h"
#include "Platform/Platform.h"
#include "Core/Log.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Antomic
{
    void Scene::AddDrawable(const Ref<Drawable> &drawable)
    {
        mDrawables.push_back(drawable);
    }

    void Scene::Update(const uint32_t &time)
    {
        int8_t dx = 0;
        int8_t dy = 0;
        int8_t dz = 0;

        dx = Platform::IsKeyPressed(Key::KeyA) ? 1 : (Platform::IsKeyPressed(Key::KeyD) ? -1 : dx);
        dy = Platform::IsKeyPressed(Key::KeyW) ? 1 : (Platform::IsKeyPressed(Key::KeyS) ? -1 : dy);
        dz = Platform::IsKeyPressed(Key::KeyR) ? 1 : (Platform::IsKeyPressed(Key::KeyF) ? -1 : dz);

        auto cPosition = mActiveCamera->GetPosition();
        cPosition += (((float)time / 1000.f) * glm::vec3(dx, dy, dz));
        auto lookat = cPosition - glm::vec3(0, 0, 1);

        mActiveCamera->SetPosition(cPosition);
        mViewMatrix = glm::lookAt(
            mActiveCamera->GetPosition(),
            lookat,
            glm::vec3(0, 1, 0));
    }

    void Scene::Load()
    {
        // For now hardcoded
        CameraFrustum f = {0.1f, 100.f};
        mType = SceneType::SCENE_3D;
        mActiveCamera = CreateRef<PerspetiveCamera>(f, 45.0f);
        mActiveCamera->SetPosition({0, 0, 3});
        mViewMatrix = glm::lookAt(
            mActiveCamera->GetPosition(),
            glm::vec3(0, 0, 2),
            glm::vec3(0, 1, 0));
    }

    void Scene::Unload()
    {
    }

    void Scene::SubmitDrawables(const Ref<RendererFrame> &frame)
    {
        for (auto drawable : mDrawables)
        {
            frame->QueueDrawable(drawable);
        }
    }

}