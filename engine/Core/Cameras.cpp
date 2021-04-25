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
#include "Core/Cameras.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Antomic
{
    Camera::Camera()
    {
    }

    Camera::~Camera()
    {
    }

    Ref<Camera> Camera::CreatePerspective(float fov, float aspect, float near, float far)
    {
        auto camera = CreateRef<Camera>();
        camera->SetProjection(glm::perspective(fov, aspect, near, far));
        return camera;
    }

    Ref<Camera> Camera::CreateOrtographic(float left, float right, float bottom, float top, float near, float far)
    {
        auto camera = CreateRef<Camera>();
        camera->SetProjection(glm::ortho(left, right, bottom, top, near, far));
        return camera;
    }

    void Camera::UpdateCamera(const Ref<Camera> &camera, const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up)
    {
        camera->SetView(glm::lookAt(position, target, up));
    }

    void Camera::UpdateCamera(const Ref<Camera> &camera, float fov, float aspect, float near, float far)
    {
        camera->SetProjection(glm::perspective(fov, aspect, near, far));
    }

    void Camera::UpdateCamera(const Ref<Camera> &camera, float left, float right, float bottom, float top, float near, float far)
    {
        camera->SetProjection(glm::ortho(left, right, bottom, top, near, far));
    }

}