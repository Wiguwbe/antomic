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
#ifdef ANTOMIC_SDL_PLATFORM
#pragma once
#include "Platform/Input.h"
#include "SDL2/SDL.h"

namespace Antomic
{
	struct KeyState
	{
		KeyState()
			: Modifiers(0)
		{
			for (uint32_t i = 0; i < Key::Count; ++i)
			{
				Keys[i] = false;
			}
		}
		uint8_t Modifiers;
		bool Keys[Key::Count];
	};

	struct MouseState
	{
		MouseState()
			: X(0), Y(0), Z(0)
		{
			for (uint32_t i = 0; i < MouseButton::Count; ++i)
			{
				Buttons[i] = false;
			}
		}

		int32_t X;
		int32_t Y;
		int32_t Z;
		bool Buttons[MouseButton::Count];
	};

	struct GamepadState
	{
		GamepadState()
		{
			for (uint32_t i = 0; i < GamepadAxis::Count; ++i)
			{
				maxis[i] = 0;
			}
		}

		int32_t maxis[GamepadAxis::Count];
	};

    class InputSDL : public Input
    {
    public:
        InputSDL() = default;
        virtual ~InputSDL() override = default;

    public:
        virtual bool IsKeyPressed(Key::Enum key) override;
        virtual bool IsMouseButtonPressed(MouseButton::Enum button) override;
        virtual uint8_t GetKeyModifiers() override;
        virtual glm::vec3 GetMousePosition() override;

    public:
        static void InitMappings();
        static uint8_t TranslateKeyModifiers(uint16_t key);
        static uint8_t TranslateKeyModifierPress(uint16_t _key);
    };

} // namespace Antomic
#endif