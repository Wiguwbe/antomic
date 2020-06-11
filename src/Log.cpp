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

Log::Log(std::ostream *stream, Level level) : mStream(stream), mLevel(level)
{
}

void Log::setLevel(Level level)
{
    this->mLevel = level;
}

void Log::info(const char *message)
{
    if (this->mLevel >= Log::Info)
        *this->mStream << "[INFO][" << now() << "] " << message << std::endl;
    ;
}

void Log::warn(const char *message)
{
    if (this->mLevel >= Log::Warn)
        *this->mStream << "[WARN][" << now() << "] " << message << std::endl;
    ;
}

void Log::error(const char *message)
{
    if (this->mLevel >= Log::Error)
        *this->mStream << "[ERROR][" << now() << "] " << message << std::endl;
};

ConsoleLog::ConsoleLog() : Log(&std::cout, Log::Info)
{
}

FileLog::FileLog() : Log(&mFileStream, Log::Info)
{
    mFileStream.open("game.log", std::ios::app );
}

FileLog::~FileLog()
{
    mFileStream.close();
}