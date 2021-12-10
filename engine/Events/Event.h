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
#include "Core/Log.h"
#include "Profiling/Instrumentor.h"

namespace Antomic
{
	enum class EventType
	{
		None = 0,
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,
		AppTick,
		AppUpdate,
		AppRender,
		KeyPressed,
		KeyReleased,
		KeyTyped,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled,
		GamepadButtonPressed,
		GamepadButtonReleased,
		GamepadAxisMoved
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4),
		EventCategoryGamepad = BIT(5),
		EventCategoryGamepadButton = BIT(6)
	};

#define EVENT_CLASS_TYPE(type)                                                                     \
	static EventType GetStaticType()                                                               \
	{                                                                                              \
		return EventType::type;                                                                    \
	}                                                                                              \
	virtual EventType GetEventType() const override                                                \
	{                                                                                              \
		return GetStaticType();                                                                    \
	}                                                                                              \
	virtual const char* GetName() const override                                                   \
	{                                                                                              \
		return #type;                                                                              \
	}

#define EVENT_CLASS_CATEGORY(category)                                                             \
	virtual int GetCategoryFlags() const override                                                  \
	{                                                                                              \
		return category;                                                                           \
	}

	class Event
	{
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const
		{
			return GetName();
		}

		bool IsCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

		inline bool IsHandled() const
		{
			return mHandled;
		}

		inline void SetHandled(bool handled)
		{
			mHandled = handled;
		}

	protected:
		bool mHandled = false;
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: mEvent(event)
		{ }

		// F will be deduced by the compiler
		template <typename T, typename F>
		bool Dispatch(const F& func)
		{
			if(mEvent.GetEventType() == T::GetStaticType())
			{
				mEvent.SetHandled(func(static_cast<T&>(mEvent)));
				return true;
			}
			return false;
		}

	private:
		Event& mEvent;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
} // namespace Antomic