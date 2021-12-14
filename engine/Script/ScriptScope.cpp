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
#include "ScriptScope.h"
#include <memory>

namespace Antomic
{
	ScriptScope::ScriptScope(const Ref<ScriptScope>& parent)
		: mParent(parent)
	{ }

	ScriptScope::~ScriptScope()
	{
		CleanUp();
	}

	Ref<ScriptObject> ScriptScope::GetName(const std::string name)
	{
		auto search = mNames.find(name);
		if(search != mNames.end())
			return search->second;

		if(mParent != nullptr)
			return mParent->GetName(name);

		return nullptr;
	}

	void ScriptScope::SetName(const std::string name, Ref<ScriptObject>& object)
	{
		mNames[name] = object;
	}

	Ref<ScriptScope> ScriptScope::CleanUp()
	{
		auto scope = mParent;
		mParent = nullptr;
		mNames.clear();
		return scope;
	}

	Ref<ScriptScope> ScriptScope::NewScope()
	{
		return CreateRef<ScriptScope>(shared_from_this());
	}

} // namespace Antomic
