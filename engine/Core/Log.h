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
#include <fstream>
#include <string>
#include "Singleton.h"

#ifdef NDEBUG
#define Logger FileLog::GetInstance()
#else
#define Logger ConsoleLog::GetInstance()
#endif

namespace Engine {

    class Log
    {
    public:
        enum Level
        {
            ErrorLevel = 0,
            WarnLevel,
            InfoLevel
        };

    public:
        Log(std::ostream *stream, Level level);

        void SetLevel(Level level);
        void Info(const char *message);
        void Info(std::string message);
        void Warn(const char *message);
        void Warn(std::string message);
        void Error(const char *message);
        void Error(std::string message);

    private:
        std::ostream *mStream;
        int mLevel;
    };

    class ConsoleLog : public Log, public Singleton<ConsoleLog>
    {
    public:
        ConsoleLog();
    };
    /*
    Log to file
    */
    class FileLog : public Log, public Singleton<FileLog>
    {

    public:
        FileLog();
        ~FileLog();

    public:
        std::ofstream mFileStream;
    };
}
