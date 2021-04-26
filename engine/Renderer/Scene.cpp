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
#include "Renderer/RendererWorker.h"
#include "Renderer/RendererFrame.h"
#include "Core/Cameras.h"
#include "Core/Log.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Antomic
{
    Scene::Scene(SceneType type)
        : mCameraMoveDirection({0, 0, 0}),
          mCameraPosition({0, 0, 3}),
          mCameraLookAt({0, 0, 0}),
          mDirty(true),
          mSceneType(type)
    {
    }

    Scene::~Scene()
    {
    }

    glm::mat4 Scene::GetProjectionMatrix(uint32_t width, uint32_t height)
    {
        if ( mSceneType == SceneType::SCENE_2D )
            return glm::ortho(0.f, (float)width,0.f,(float)height, 0.1f, 100.0f);

        return glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    }

    const glm::mat4 &Scene::GetViewMatrix()
    {
        if (mDirty)
        {
            mViewMatrix = glm::lookAt(
                mCameraPosition,
                mCameraLookAt,
                glm::vec3(0, 1, 0));
        }

        return mViewMatrix;
    }

    void Scene::SetPosition(const glm::vec3 &position)
    {
        mCameraPosition = position;
        mDirty = true;
    }

    void Scene::SetLookAt(const glm::vec3 &lookat)
    {
        mCameraLookAt = lookat;
        mDirty = true;
    }

    void Scene::SetMoveDirection(const glm::vec3 &direction)
    {
        mCameraMoveDirection = direction;
    }

    void Scene::AddDrawable(const Ref<Drawable> &drawable)
    {
        mDrawables.push_back(drawable);
    }

    void Scene::Update(const uint32_t &time)
    {
        mCameraPosition += (((float)time / 1000.f) * mCameraMoveDirection);
        mCameraLookAt += (((float)time / 1000.f) * mCameraMoveDirection);
    }

    void Scene::Load()
    {
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