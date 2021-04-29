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
#include "Renderer/Mesh.h"
#include "Renderer/Bindable.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

namespace Antomic
{
    Mesh::Mesh(const Ref<VertexArray> &vertexArray, const Ref<Material> &material)
        : mVertexArray(vertexArray), mMaterial(material), mMatrix(1.0f)
    {
    }

    void Mesh::AddBindable(const Ref<Texture> &texture)
    {
        mBindables.push_back(std::dynamic_pointer_cast<Bindable>(texture));
    }

    void Mesh::AddBindable(const Ref<Material> &material)
    {
        mBindables.push_back(std::dynamic_pointer_cast<Shader>(material));
    }

} // namespace Antomic
