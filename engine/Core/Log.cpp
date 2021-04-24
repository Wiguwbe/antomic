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
#include "Core/Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Antomic
{
    Ref<spdlog::logger> Log::s_Logger;

    void Log::Init()
    {

        std::vector<spdlog::sink_ptr> logSinks;

        // By default we have only a file logger
        logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("engine.log", true));
        logSinks[0]->set_pattern("[%T] [%l] %n: %v");

#ifdef ANTOMIC_DEBUG
        // For debugging purposes we attach a logger to the console
        logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logSinks[1]->set_pattern("%^[%T] %n: %v%$");
#endif

        s_Logger = std::make_shared<spdlog::logger>("Antomic", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_Logger);
        s_Logger->set_level(spdlog::level::trace);
        s_Logger->flush_on(spdlog::level::trace);
    }
} // namespace Antomic
