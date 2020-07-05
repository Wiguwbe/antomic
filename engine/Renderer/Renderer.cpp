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
#include "Renderer/Renderer.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

namespace Engine
{

    void Renderer::Init(uint32_t width, uint32_t height)
    {
        bgfx::renderFrame();
        bgfx::init();
        OnWindowResize(width, height);
    }

    void Renderer::Shutdown()
    {
        bgfx::shutdown();
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        bgfx::reset(width, height, BGFX_RESET_VSYNC);
        bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
        bgfx::setViewClear(0,
                           BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                           0x100000, 1.0f, 0);
    }

    void Renderer::RenderFrame()
    {
        bgfx::touch(0);
        BeginScene();
        EndScene();
        bgfx::frame();
    }

    void Renderer::BeginScene()
    {
    }

    void Renderer::EndScene()
    {
    }

} // namespace Engine