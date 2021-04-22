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
#include "Input/Input.h"

namespace Antomic
{
    class KeyEvent : public Event
    {
    public:
        Key::Enum GetKeyCode() const { return m_KeyCode; }
        uint8_t GetKeyModifiers() const { return m_Modifiers; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        KeyEvent(Key::Enum keycode, uint8_t modifiers)
            : m_KeyCode(keycode), m_Modifiers(modifiers) {}

        Key::Enum m_KeyCode;
        uint8_t m_Modifiers;
    };

    class KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(Key::Enum keycode, uint8_t modifiers, int repeat)
            : KeyEvent(keycode, modifiers), m_Repeat(repeat) {}

        int GetRepeatCount() const { return m_Repeat; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << m_KeyCode << " ( repeat:" << m_Repeat << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        bool m_Repeat;
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(Key::Enum keycode, uint8_t modifiers)
            : KeyEvent(keycode, modifiers) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << m_KeyCode;
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
            ss << "KeyTypedEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };
} // namespace Antomic