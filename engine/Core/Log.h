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
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Antomic
{

    class Log
    {
    public:
        static void Init();
        static Ref<spdlog::logger> &GetLogger() { return sLogger; }

    private:
        static Ref<spdlog::logger> sLogger;
    };

} // namespace Antomic

// log macros
#define ANTOMIC_TRACE(...) ::Antomic::Log::GetLogger()->trace(__VA_ARGS__)
#define ANTOMIC_INFO(...) ::Antomic::Log::GetLogger()->info(__VA_ARGS__)
#define ANTOMIC_WARN(...) ::Antomic::Log::GetLogger()->warn(__VA_ARGS__)
#define ANTOMIC_ERROR(...) ::Antomic::Log::GetLogger()->error(__VA_ARGS__)
#define ANTOMIC_CRITICAL(...) ::Antomic::Log::GetLogger()->critical(__VA_ARGS__)
