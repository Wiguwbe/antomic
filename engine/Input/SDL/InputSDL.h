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
#include "Input/Input.h"
#ifdef ENGINE_PLATFORM_LINUX
#include <SDL2/SDL.h>
#elif ENGINE_PLATFORM_WINDOWS
#include "SDL.h"
#endif

namespace Engine
{

    class InputSDL : public Input
    {
    public:
        InputSDL();
        virtual ~InputSDL();

        virtual bool SetupInput();
        virtual void ProcessInputEvents();
        virtual void SetEventHandler(const EventHandler &handler) { m_Handler = handler; }

    protected:
        EventHandler m_Handler;
    };

} // namespace Engine