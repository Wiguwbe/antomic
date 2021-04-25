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
    enum class CameraType
    {
        ORTOGRAPHIC,
        PERSPECTIVE
    };

    class Camera
    {
    public:
        Camera();
        virtual ~Camera();

    public:
        // Common methods
        inline const glm::mat4 &GetProjection() const { return mProj; }
        inline const glm::mat4 &GetView() const { return mView; }
        CameraType GetType() { return mType; }

        inline bool IsDirty() const { return mDirty; }
        inline void ClearDirty() { mDirty = false; }

    public:
        static Ref<Camera> CreatePerspective(float fov, float aspect, float near, float far);
        static Ref<Camera> CreateOrtographic(float left, float right, float bottom, float top, float near, float far);
        static void UpdateCamera(const Ref<Camera> &camera, float fov, float aspect, float near, float far);
        static void UpdateCamera(const Ref<Camera> &camera, float left, float right, float bottom, float top, float near, float far);
        static void UpdateCamera(const Ref<Camera> &camera, const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up);

    private:
        inline void SetProjection(const glm::mat4 &proj)
        {
            mProj = proj;
            mDirty = true;
        }
        inline void SetView(const glm::mat4 &view)
        {
            mView = view;
            mDirty = true;
        }

    private:
        CameraType mType;
        glm::mat4 mView, mProj;
        bool mDirty;
    };

} // namespace Antomic
