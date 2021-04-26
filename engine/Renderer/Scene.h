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
        Scene();
        virtual ~Scene();

    public:
        const glm::mat4 &GetViewMatrix();

        inline const glm::vec3 &GetPosition() const { return mCameraPosition; }
        inline const glm::vec3 &GetLookAt() const { return mCameraLookAt; }
        inline const glm::vec3 &GetMoveDirection() const { return mCameraMoveDirection; }

        void SetPosition(const glm::vec3 &position);
        void SetLookAt(const glm::vec3 &lookat);
        void SetMoveDirection(const glm::vec3 &direction);

        void AddDrawable(const Ref<Drawable> &drawable);
        void Load();
        void Unload();

        void Update(const uint32_t &time);
        virtual void SubmitDrawables(const Ref<RendererFrame> &frame);

    private:
        VectorRef<Drawable> mDrawables;
        glm::vec3 mCameraMoveDirection;
        glm::vec3 mCameraPosition;
        glm::vec3 mCameraLookAt;
        glm::mat4 mViewMatrix;
        bool mDirty;
    };

} // namespace Antomic
