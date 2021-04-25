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
#include "Core/Cameras.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Drawable.h"
#include "Renderer/Scene.h"
#include "Renderer/RendererFrame.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Antomic
{
    const Ref<RendererFrame> Renderer::SubmitScene(uint32_t width, uint32_t height, const Ref<Scene> &scene)
    {
        auto frame = CreateRef<RendererFrame>(scene->GetProjection(), scene->GetView(), width, height);
        scene->SubmitDrawables(frame);
        return frame;
    }

    void Renderer::Submit(const Ref<RendererFrame> &frame, const Ref<Drawable> &drawable)
    {
        frame->QueueDrawable(drawable);
    }

    void Renderer::Flush(const Ref<RendererFrame> &frame)
    {
        RenderCommand::SetViewport(0, 0, frame->GetWidth(), frame->GetHeight());
        RenderCommand::SetClearColor({0.5f, 0.1f, 0.8f, 1.0f});
        RenderCommand::Clear();

        auto m_proj = frame->GetProjection();
        auto m_view = frame->GetView();
        auto m_projview = m_proj * m_view;

        while (!frame->Empty())
        {
            auto drawable = frame->PopDrawable();
            auto shader = drawable->GetShader();
            auto va = drawable->GetVertexArray();
            auto m_model = drawable->GetMatrix();

            shader->SetUniformValue("m_proj", m_proj);
            shader->SetUniformValue("m_view", m_view);
            shader->SetUniformValue("m_model", m_model);
            shader->SetUniformValue("m_projview", m_projview);

            shader->Bind();
            RenderCommand::DrawIndexed(va);
        }
    }

} // namespace Anatomic