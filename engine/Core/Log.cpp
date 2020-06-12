#include "Log.h"
#include "iostream"
#include <chrono>
#include <string>

static std::string now()
{
    std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string ts = std::ctime(&t);
    ts.resize(ts.size() - 1);
    return ts;
}

Engine::Log::Log(std::ostream *stream, Level level) : mStream(stream), mLevel(level)
{
}

void Engine::Log::SetLevel(Level level)
{
    this->mLevel = level;
}

void Engine::Log::Info(const char *message)
{
    if (this->mLevel >= Log::InfoLevel)
        *this->mStream << "[INFO][" << now() << "] " << message << std::endl;
}

void Engine::Log::Info(std::string message)
{
    this->Info(message.c_str());
}

void Engine::Log::Warn(const char *message)
{
    if (this->mLevel >= Log::WarnLevel)
        *this->mStream << "[WARN][" << now() << "] " << message << std::endl;
}

void Engine::Log::Warn(std::string message)
{
    this->Warn(message.c_str());
}

void Engine::Log::Error(const char *message)
{
    if (this->mLevel >= Log::ErrorLevel)
        *this->mStream << "[ERROR][" << now() << "] " << message << std::endl;
}

void Engine::Log::Error(std::string message)
{
    this->Error(message.c_str());
}

Engine::ConsoleLog::ConsoleLog() : Log(&std::cout, Log::InfoLevel)
{
}

Engine::FileLog::FileLog() : Log(&mFileStream, Log::InfoLevel)
{
    mFileStream.open("game.log", std::ios::app );
}

Engine::FileLog::~FileLog()
{
    mFileStream.close();
}