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
#include "Events/Event.h"
#include "Platform/Input.h"

namespace Antomic
{
    class KeyEvent : public Event
    {
    public:
        Key::Enum GetKeyCode() const { return mKeyCode; }
        uint8_t GetKeyModifiers() const { return mModifiers; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        KeyEvent(Key::Enum keycode, uint8_t modifiers)
            : mKeyCode(keycode), mModifiers(modifiers) {}

        Key::Enum mKeyCode;
        uint8_t mModifiers;
    };

    class KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(Key::Enum keycode, uint8_t modifiers, int repeat)
            : KeyEvent(keycode, modifiers), mRepeat(repeat) {}

        int GetRepeatCount() const { return mRepeat; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << mKeyCode << " ( repeat:" << mRepeat << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        bool mRepeat;
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(Key::Enum keycode, uint8_t modifiers)
            : KeyEvent(keycode, modifiers) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << mKeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent(Key::Enum keycode, uint8_t modifiers)
            : KeyEvent(keycode, modifiers) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << mKeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };
} // namespace Antomic