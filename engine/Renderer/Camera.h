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
#include "Renderer/Renderer.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Antomic
{
    struct CameraFrustum
    {
        float Near;
        float Far;
    };

    enum class CameraType
    {
        NONE,
        ORTOGRAPHIC,
        PERSPECTIVE,
    };

    /*************************************************************
     * Camera Implementation
     *************************************************************/

    class Camera
    {
    public:
        Camera() = default;
        virtual ~Camera() = default;

    public:
        virtual CameraType GetType() const { return CameraType::NONE; }
        virtual const CameraFrustum GetFrustum() const { return {1.f, 1.f}; }
        const glm::vec3 &GetPosition() const { return mPosition; }
        void SetPosition(const glm::vec3 &position) { mPosition = position; }
        virtual const glm::mat4 GetProjectionMatrix(const RendererViewport &viewport) { return glm::mat4(1.f); }

    private:
        glm::vec3 mPosition;
    };

    /*************************************************************
     * OrthographicCamera Implementation
     *************************************************************/

    class OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera() = default;
        virtual ~OrthographicCamera() = default;

    public:
        virtual CameraType GetType() const override { return CameraType::ORTOGRAPHIC; }
        virtual const glm::mat4 GetProjectionMatrix(const RendererViewport &viewport) override
        {
            return glm::ortho((float)viewport.Left, (float)viewport.Top, (float)viewport.Right, (float)viewport.Bottom, -1.f, 1.f);
        }
    };

    /*************************************************************
     * PerspetiveCamera Implementation
     *************************************************************/

    class PerspetiveCamera : public Camera
    {
    public:
        PerspetiveCamera(const CameraFrustum &frustum, float fov) : mFrustum(frustum), mFoV(fov) {}
        virtual ~PerspetiveCamera() = default;

    public:
        virtual CameraType GetType() const override { return CameraType::PERSPECTIVE; }
        void SetFrustum(const CameraFrustum &frustum) { mFrustum = frustum; }
        float GetFoV() const { return mFoV; }
        virtual const glm::mat4 GetProjectionMatrix(const RendererViewport &viewport) override
        {
            return glm::perspective(glm::radians(mFoV), (float)(viewport.Right - viewport.Left) / (float)(viewport.Bottom - viewport.Top), mFrustum.Near, mFrustum.Far);
        }

    private:
        CameraFrustum mFrustum;
        float mFoV;
    };

} // namespace Antomic
