
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
#include "Script/Parser/Reader.h"
#include "Core/Log.h"

namespace Antomic
{

	char TextReader::Peek()
	{
		ANTOMIC_ASSERT(!mStream.bad(), "Stream is bad!");
		return mStream.peek();
	}

	char TextReader::Read()
	{
		ANTOMIC_ASSERT(!mStream.bad(), "Stream is bad!");
		char c;
		mStream.read(&c, 1);
		return c;
	}

	char TextReader::Read(std::string& out)
	{
		auto c = Read();
		out += c;
		return c;
	}

	char FileReader::Peek()
	{
		ANTOMIC_ASSERT(!mStream.bad(), "Stream is bad");
		return mStream.peek();
	}

	char FileReader::Read()
	{
		ANTOMIC_ASSERT(!mStream.bad(), "Stream is bad!");
		char c;
		mStream.read(&c, 1);
		return c;
	}

	char FileReader::Read(std::string& out)
	{
		auto c = Read();
		out += c;
		return c;
	}

	Ref<Reader> Reader::FromFile(const std::string& name)
	{
		return CreateRef<FileReader>(name);
	}

	Ref<Reader> Reader::FromString(const std::string& string, const std::string& name)
	{
		return CreateRef<TextReader>(string, name);
	}

} // namespace Antomic