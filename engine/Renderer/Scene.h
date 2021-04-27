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
#include "glm/glm.hpp"

namespace Antomic
{
    enum class SceneType
    {
        NONE,
        SCENE_2D,
        SCENE_3D,
    };

    class Scene
    {
    public:
        Scene() = default;
        virtual ~Scene() = default;

    public:
        inline const glm::mat4 &GetViewMatrix() const { return mViewMatrix; }
        inline const Ref<Camera> &GetActiveCamera() const { return mActiveCamera; }
        inline const SceneType &GetType() const { return mType; }

        void AddDrawable(const Ref<Drawable> &drawable);
        void Load();
        void Unload();

        void Update(const uint32_t &time);
        virtual void SubmitDrawables(const Ref<RendererFrame> &frame);

    private:
        VectorRef<Drawable> mDrawables;
        glm::mat4 mViewMatrix;
        Ref<Camera> mActiveCamera;
        SceneType mType;
    };

} // namespace Antomic
