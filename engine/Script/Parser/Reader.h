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

namespace Antomic
{
    class Reader
    {
    public:
        virtual ~Reader() = default;

        virtual bool IsEOF() = 0;
        virtual char Peek() = 0;
        virtual char Read() = 0;
        virtual char Read(std::string &out) = 0;
        virtual const std::string &GetFileName() const = 0;

    public:
        static Ref<Reader> FromFile(const std::string &name);
        static Ref<Reader> FromString(const std::string &string, const std::string &name);
    };

    class TextReader : public Reader
    {
    public:
        TextReader(const std::string &string, const std::string &name) : mStream(string), mName(name) {}
        virtual ~TextReader() = default;

        virtual bool IsEOF() override 
        {   
            return mStream.peek() == EOF; 
        }
        virtual char Peek() override;
        virtual char Read() override;
        virtual char Read(std::string &out) override;
        virtual const std::string &GetFileName() const { return mName; }

    private:
        std::stringstream mStream;
        std::string mName;
    };

    class FileReader : public Reader
    {
    public:
        FileReader(const std::string &name) : mStream(name) {}
        virtual ~FileReader() { mStream.close(); }

        virtual bool IsEOF() override { return mStream.peek() == EOF; }
        virtual char Peek() override;
        virtual char Read() override;
        virtual char Read(std::string &out) override;
        virtual const std::string &GetFileName() const { return mName; }

    private:
        std::ifstream mStream;
        std::string mName;
        bool mEof = false;
    };

}