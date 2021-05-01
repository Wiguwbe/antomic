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
#include "Renderer/Drawable.h"
#include "Renderer/Bindable.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/RenderCommand.h"

namespace Antomic
{
    Drawable::Drawable(const Ref<VertexArray> &vertexArray, const Ref<Material> &material)
        : mVertexArray(vertexArray), mMaterial(material), mMatrix(1.0f)
    {
    }

    void Drawable::AddBindable(const Ref<Bindable> &bindable)
    {
        mBindables.push_back(bindable);
    }

    void Drawable::Draw()
    {
        auto shader = GetShader();
        auto va = GetVertexArray();
        auto m_model = GetModelMatrix();

        for (auto bindable : GetBindables())
        {
            bindable->Bind();
        }

        shader->SetUniformValue("m_model", m_model);
        shader->Bind();
        RenderCommand::DrawIndexed(va);
    }

} // namespace Antomic
