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
#include "Renderer/Renderer.h"
#include "Renderer/RendererFrame.h"
#include "Core/Cameras.h"
#include "Core/Log.h"

namespace Antomic
{
    Scene::Scene(const Ref<Camera> &camera)
    {
        ANTOMIC_ASSERT(camera, "Scene: Camera is null")
        mCamera = camera;
    }

    Scene::~Scene()
    {
    }

    void Scene::AddDrawable(const Ref<Drawable> &drawable)
    {
        mDrawables.push_back(drawable);
    }

    void Scene::Update(const uint32_t &time)
    {
        
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

        Renderer::Flush(frame);
    }
}