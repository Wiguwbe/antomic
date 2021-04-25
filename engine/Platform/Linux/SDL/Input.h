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
#ifdef ANTOMIC_LINUX_SDL_PLATFORM
#pragma once
#include "Platform/Input.h"
#include "SDL2/SDL.h"

namespace Antomic
{

    class InputSDL : public Input
    {
    public:
        InputSDL();
        virtual ~InputSDL();

    public:
        virtual bool SetupInput() override;
        virtual void ProcessEvents() override;
        virtual void SetEventHandler(const EventHandler &handler) override { mHandler = handler; }

    protected:
        EventHandler mHandler;
    };

} // namespace Antomic
#endif