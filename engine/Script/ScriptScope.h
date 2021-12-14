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
#include "ScriptObject.h"

namespace Antomic
{
	class ScriptScope : public std::enable_shared_from_this<ScriptScope>
	{
	public:
		ScriptScope(const Ref<ScriptScope>& parent = nullptr);
		~ScriptScope();

	public:
		Ref<ScriptObject> GetName(const std::string name);
		void SetName(const std::string name, Ref<ScriptObject>& object);
		Ref<ScriptScope> NewScope();
		Ref<ScriptScope> CleanUp();

	private:
		Ref<ScriptScope> mParent;
		std::map<std::string, Ref<ScriptObject>> mNames;
	};
} // namespace Antomic