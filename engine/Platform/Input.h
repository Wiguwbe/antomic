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
#include "glm/glm.hpp"

namespace Antomic
{
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
			Count
		};
	};

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

	class Input
	{
	public:
		virtual ~Input() = default;

	public:
		virtual bool IsKeyPressed(Key::Enum key) = 0;
		virtual bool IsMouseButtonPressed(MouseButton::Enum button) = 0;
		virtual uint8_t GetKeyModifiers() = 0;
		virtual glm::vec3 GetMousePosition() = 0;

	public:
		inline static void AddKeyTranslation(uint16_t src, Antomic::Key::Enum tgt)
		{
			sKeyForwardMap[src & 0xff] = (uint8_t)tgt;
			sKeyReverseMap[tgt & 0xff] = (uint8_t)src;
		}

		inline static int FromKey(Antomic::Key::Enum key)
		{
			return (int)sKeyReverseMap[key & 0xff];
		}

		inline static Antomic::Key::Enum ToKey(int key)
		{
			return (Antomic::Key::Enum)sKeyForwardMap[key & 0xff];
		}

		inline static void ClearKeyMap()
		{
			std::memset(sKeyForwardMap, 0, sizeof(sKeyForwardMap));
			std::memset(sKeyReverseMap, 0, sizeof(sKeyReverseMap));
		}

	private:
		static uint8_t sKeyForwardMap[256];
		static uint8_t sKeyReverseMap[256];
	};

} // namespace Antomic