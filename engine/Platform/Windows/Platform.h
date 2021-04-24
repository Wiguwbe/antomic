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
#ifdef ENGINE_PLATFORM_WINDOWS
#pragma once
#include "Platform/Platform.h"

namespace Antomic
{

    class WindowsPlatform : public Platform
    {
    public:
        WindowsPlatform();
        virtual ~WindowsPlatform() override;

        // Windows Operations
        virtual Scope<Window> CreateWindow(uint32_t width, uint32_t height, std::string title, RenderAPI api = RenderAPI::OPENGL) override;
		virtual Scope<Input> CreateInput() override;

        // Time Operations
        virtual uint32_t GetTicks() const override;
    private:
#ifdef ENGINE_CHRONO_SUPPORT
        std::chrono::time_point<std::chrono::high_resolution_clock> mPlatformStart;
#endif
    };
} // namespace Antomic
#endif