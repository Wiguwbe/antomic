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
#include "Core/Input.h"
#include "Events/Event.h"

namespace Engine {

	struct WindowHandle  { uint16_t idx; };

	inline bool isValid(WindowHandle _handle)  { return UINT16_MAX != _handle.idx; }

	struct WindowState
	{
		WindowState()
			: m_width(0)
			, m_height(0)
			, m_nwh(NULL)
		{
			m_handle.idx = UINT16_MAX;
		}

		WindowHandle m_handle;
		uint32_t     m_width;
		uint32_t     m_height;
		MouseState   m_mouse;
		void*        m_nwh;
	};    
}