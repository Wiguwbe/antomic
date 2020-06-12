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
#include "Core/Input.h"

namespace Engine
{
    class GamepadAxisEvent : public Event
    {
    public:
        GamepadAxisEvent(float x, float y)
            : m_GamepadX(x), m_GamepadY(y) {}

        float GetX() const { return m_GamepadX; }
        float GetY() const { return m_GamepadY; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseMovedEvent: " << m_GamepadX << ", " << m_GamepadY;
            return ss.str();
        }

        EVENT_CLASS_TYPE(GamepadAxisMoved)
        EVENT_CLASS_CATEGORY(EventCategoryGamepad | EventCategoryInput)
    private:
        float m_GamepadX, m_GamepadY;
    };

    class GamepadButtonEvent : public Event
    {
    public:
        inline GamepadCode GetGamepadButton() const { return m_Button; }

        EVENT_CLASS_CATEGORY(EventCategoryGamepadButton | EventCategoryGamepad | EventCategoryInput)
    protected:
        GamepadButtonEvent(GamepadCode button)
            : m_Button(button) {}

        GamepadCode m_Button;
    };

    class GamepadButtonPressedEvent : public GamepadButtonEvent
    {
    public:
        GamepadButtonPressedEvent(GamepadCode button)
            : GamepadButtonEvent(button) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "GamepadButtonPressedEvent: " << m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(GamepadButtonPressed)
    };

    class GamepadButtonReleasedEvent : public GamepadButtonEvent
    {
    public:
        GamepadButtonReleasedEvent(GamepadCode button)
            : GamepadButtonEvent(button) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "GamepadButtonReleasedEvent: " << m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(GamepadButtonReleased)
    };

} // namespace Hazel