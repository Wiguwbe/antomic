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
#include "Renderer/Material.h"
#include "Renderer/Shader.h"

namespace Antomic
{
	class BasicMaterial : public Material
	{
	public:
		BasicMaterial();
		virtual ~BasicMaterial() = default;

	public:
		virtual void Bind() const;
		virtual void Unbind() const
		{
			mShader->Unbind();
		}
		virtual const Ref<Shader>& GetShader() const
		{
			return mShader;
		};

	private:
		Ref<Shader> mShader;
	};
} //namespace Antomic