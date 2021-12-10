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
#include "Core/LayerStack.h"
#include "Events/Event.h"

namespace Antomic
{
	LayerStack::LayerStack() { }

	LayerStack::~LayerStack() { }

	void LayerStack::PushBack(Ref<Layer> l)
	{
		if(mStack.empty())
		{
			PushFront(l);
			return;
		}

		l->OnAttach();
		mStack.insert(mStack.begin(), l);
	}

	void LayerStack::PushFront(Ref<Layer> l)
	{
		l->OnAttach();
		mStack.push_back(l);
	}

	void LayerStack::PopBack()
	{
		if(mStack.empty())
			return;
		mStack.back()->OnDetach();
		mStack.pop_back();
	}

	void LayerStack::PopFront()
	{
		if(mStack.empty())
			return;
		mStack.back()->OnDetach();
		mStack.erase(mStack.begin());
	}

	void LayerStack::Remove(Ref<Layer> l)
	{
		auto it = std::find(mStack.begin(), mStack.end(), l);
		if(it != mStack.end())
		{
			(*it)->OnDetach();
			mStack.erase(it);
		}
	}

	void LayerStack::Update(const uint32_t& time)
	{
		for(auto it = mStack.begin(); it != mStack.end(); ++it)
		{
			if((*it)->IsEnabled())
				(*it)->Update(time);
		}
	}

	void LayerStack::Submit()
	{
		for(auto it = mStack.begin(); it != mStack.end(); ++it)
		{
			if((*it)->IsEnabled())
				(*it)->Submit();
		}
	}

	void LayerStack::OnEvent(Event& e)
	{
		for(auto it = mStack.rbegin(); it != mStack.rend(); ++it)
		{
			if((*it)->IsEnabled())
			{
				(*it)->OnEvent(e);
				if(e.IsHandled())
					return;
			}
		}
	}

} // namespace Antomic