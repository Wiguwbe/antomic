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
    class Renderer
    {
    public:
        static const Ref<RendererFrame> SubmitScene(uint32_t width, uint32_t height, const Ref<Scene> &scene);
        static void Submit(const Ref<RendererFrame> &frame, const Ref<Drawable> &drawable);
        static void Flush(const Ref<RendererFrame> &frame);
    };
} // namespace Antomic