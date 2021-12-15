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
#include "Renderer/Texture.h"
#include "Platform/NullRenderer/Texture.h"
#include "Platform/Platform.h"
#include "Platform/RenderAPI.h"
#ifdef ANTOMIC_GL_RENDERER
#	include "Platform/OpenGL/Texture.h"
#endif

namespace Antomic
{
	Ref<Texture> Texture::CreateTexture(uint32_t width, uint32_t height, unsigned char* data)
	{
		switch(Platform::GetRenderAPIDialect())
		{
#ifdef ANTOMIC_GL_RENDERER
			case RenderAPIDialect::OPENGL: return CreateRef<OpenGLTexture>(width, height, data);
#endif
			default: return CreateRef<NullTexture>(width, height, data);
		}
	}
} // namespace Antomic
