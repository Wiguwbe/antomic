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
#include "Core/Cameras.h"
#include "glm/glm.hpp"

namespace Antomic
{
    class Scene
    {
    public:
        Scene(const Ref<Camera> &camera);
        virtual ~Scene();

    public:
        const Ref<Camera> &GetCamera() const { return mCamera; };
        void SetCamera(const Ref<Camera> &camera) { mCamera = camera; }
        inline const glm::mat4 &GetProjection() const { return mCamera->GetProjection(); }
        inline const glm::mat4 &GetView() const { return mCamera->GetView(); }
        void AddDrawable(const Ref<Drawable> &drawable);
        void Load();
        void Unload();

        void Update(const uint32_t &time);
        virtual void SubmitDrawables(const Ref<RendererFrame> &frame);

    private:
        Ref<Camera> mCamera;
        VectorRef<Drawable> mDrawables;
    };

} // namespace Antomic
