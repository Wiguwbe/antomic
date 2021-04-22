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

namespace Antomic
{
	struct MouseButton
	{
		enum Enum
		{
			Left,
			Middle,
			Right,
			None,

			Count
		};
	};

	struct GamepadAxis
	{
		enum Enum
		{
			LeftX,
			LeftY,
			LeftZ,
			RightX,
			RightY,
			RightZ,

			Count
		};
	};

	struct Modifier
	{
		enum Enum
		{
			None = 0,
			LeftAlt = 0x01,
			RightAlt = 0x02,
			LeftCtrl = 0x04,
			RightCtrl = 0x08,
			LeftShift = 0x10,
			RightShift = 0x20,
			LeftMeta = 0x40,
			RightMeta = 0x80,
		};
	};

	struct Key
	{
		enum Enum
		{
			None = 0,
			Esc,
			Return,
			Tab,
			Space,
			Backspace,
			Up,
			Down,
			Left,
			Right,
			Insert,
			Delete,
			Home,
			End,
			PageUp,
			PageDown,
			Print,
			Plus,
			Minus,
			LeftBracket,
			RightBracket,
			Semicolon,
			Quote,
			Comma,
			Period,
			Slash,
			Backslash,
			Tilde,
			F1,
			F2,
			F3,
			F4,
			F5,
			F6,
			F7,
			F8,
			F9,
			F10,
			F11,
			F12,
			NumPad0,
			NumPad1,
			NumPad2,
			NumPad3,
			NumPad4,
			NumPad5,
			NumPad6,
			NumPad7,
			NumPad8,
			NumPad9,
			NumPadEnter,
			Key0,
			Key1,
			Key2,
			Key3,
			Key4,
			Key5,
			Key6,
			Key7,
			Key8,
			Key9,
			KeyA,
			KeyB,
			KeyC,
			KeyD,
			KeyE,
			KeyF,
			KeyG,
			KeyH,
			KeyI,
			KeyJ,
			KeyK,
			KeyL,
			KeyM,
			KeyN,
			KeyO,
			KeyP,
			KeyQ,
			KeyR,
			KeyS,
			KeyT,
			KeyU,
			KeyV,
			KeyW,
			KeyX,
			KeyY,
			KeyZ,

			GamepadA,
			GamepadB,
			GamepadX,
			GamepadY,
			GamepadThumbL,
			GamepadThumbR,
			GamepadShoulderL,
			GamepadShoulderR,
			GamepadUp,
			GamepadDown,
			GamepadLeft,
			GamepadRight,
			GamepadBack,
			GamepadStart,
			GamepadGuide,

			Count
		};
	};

	struct MouseState
	{
		MouseState()
			: m_mx(0), m_my(0), m_mz(0)
		{
			for (uint32_t i = 0; i < MouseButton::Count; ++i)
			{
				m_buttons[i] = MouseButton::None;
			}
		}

		int32_t m_mx;
		int32_t m_my;
		int32_t m_mz;
		uint8_t m_buttons[MouseButton::Count];
	};

	struct GamepadState
	{
		GamepadState()
		{
			for (uint32_t i = 0; i < GamepadAxis::Count; ++i)
			{
				m_axis[i] = 0;
			}
		}

		int32_t m_axis[GamepadAxis::Count];
	};

	class Input
	{
	protected:
		Input() = default;

	public:
		virtual bool SetupInput() = 0;
		virtual void ProcessInputEvents() = 0;
        virtual void SetEventHandler(const EventHandler& handler) = 0;

	public:
		static Scope<Input> Create();

	protected:
	};


} // namespace Antomic