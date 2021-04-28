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
    struct RendererViewport
    {
        uint32_t Left;
        uint32_t Top;
        uint32_t Right;
        uint32_t Bottom;
        glm::vec4 Color;

        RendererViewport(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
            : Left(left), Top(top), Right(right), Bottom(bottom), Color(0.0f, 0.f, 0.f, 1.f) {}

        RendererViewport(uint32_t right, uint32_t bottom)
            : Left(0), Top(0), Right(right), Bottom(bottom), Color(0.0f, 0.f, 0.f, 1.f) {}

        RendererViewport()
            : Left(0), Top(0), Right(0), Bottom(0), Color(0.0f, 0.f, 0.f, 1.f) {}
    };

    class Renderer
    {
    public:
        Renderer(const RendererViewport &viewport);
        ~Renderer() = default;

    public:
        void RenderFrame();

        const Ref<Scene> &GetCurrentScene();
        void SetCurrentScene(const Ref<Scene> &scene);

        inline const Ref<RendererFrame> GetLastFrame() const { return mLastFrame; }
        inline const glm::mat4 &GetProjectionMatrix() const { return mProjectionMatrix; }
        const uint32_t GetLastFrameTime();

        inline const RendererViewport &GetViewport() const { return mViewport; }
        void SetViewport(const RendererViewport &viewport);

    private:
        Ref<RendererFrame> mLastFrame;
        uint32_t mLastFrameTime;
        Ref<Scene> mScene;
        RendererViewport mViewport;
        glm::mat4 mProjectionMatrix;
    };
} // namespace Antomic