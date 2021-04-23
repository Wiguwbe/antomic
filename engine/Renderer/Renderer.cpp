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
#include "Core/Log.h"

namespace Antomic
{

    Renderer::Renderer(RenderAPI api) 
    {
        mApi = RendererAPI::Create(api);
    }

    Renderer::~Renderer() 
    {

    }

    void Renderer::Init(uint32_t width, uint32_t height)
    {
        OnWindowResize(width, height);
    }

    void Renderer::Shutdown()
    {
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
    }

    void Renderer::Flush()
    {
    }

    void Renderer::BeginScene()
    {
        mApi->Clear(glm::vec4(0.4f,0.3f,0.0f,1.0f));
    }

    void Renderer::EndScene()
    {
    }


}