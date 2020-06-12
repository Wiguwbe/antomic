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

static std::string now()
{
    std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string ts = std::ctime(&t);
    ts.resize(ts.size() - 1);
    return ts;
}

Engine::Log::Log(std::ostream *stream, Level level) : m_Stream(stream), m_Level(level)
{
}

void Engine::Log::SetLevel(Level level)
{
    this->m_Level = level;
}

void Engine::Log::Info(const char *message)
{
    if (this->m_Level >= Log::InfoLevel)
        *this->m_Stream << "[INFO][" << now() << "] " << message << std::endl;
}

void Engine::Log::Info(std::string message)
{
    this->Info(message.c_str());
}

void Engine::Log::Warn(const char *message)
{
    if (this->m_Level >= Log::WarnLevel)
        *this->m_Stream << "[WARN][" << now() << "] " << message << std::endl;
}

void Engine::Log::Warn(std::string message)
{
    this->Warn(message.c_str());
}

void Engine::Log::Error(const char *message)
{
    if (this->m_Level >= Log::ErrorLevel)
        *this->m_Stream << "[ERROR][" << now() << "] " << message << std::endl;
}

void Engine::Log::Error(std::string message)
{
    this->Error(message.c_str());
}

Engine::ConsoleLog::ConsoleLog() : Log(&std::cout, Log::InfoLevel)
{
}

Engine::FileLog::FileLog() : Log(&m_FileStream, Log::InfoLevel)
{
    m_FileStream.open("game.log", std::ios::app );
}

Engine::FileLog::~FileLog()
{
    m_FileStream.close();
}