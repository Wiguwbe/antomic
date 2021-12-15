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
#	include "Platform/SDL/Input.h"

namespace Antomic
{
	bool InputSDL::IsKeyPressed(Key::Enum key)
	{
		SDL_PumpEvents();
		const uint8_t* state = SDL_GetKeyboardState(NULL);
		return state[Input::FromKey(key)];
	}

	bool InputSDL::IsMouseButtonPressed(MouseButton::Enum button)
	{
		SDL_PumpEvents();
		uint32_t sdlButton = SDL_GetMouseState(nullptr, nullptr);
		uint32_t aButton;

		switch(button)
		{
			default:
			case MouseButton::Left: aButton = SDL_BUTTON_LEFT; break;
			case MouseButton::Middle: aButton = SDL_BUTTON_MIDDLE; break;
			case MouseButton::Right: aButton = SDL_BUTTON_RIGHT; break;
		}

		return sdlButton & SDL_BUTTON(aButton);
	}

	uint8_t InputSDL::GetKeyModifiers() { return 0; }

	glm::vec3 InputSDL::GetMousePosition()
	{
		int xpos, ypos;
		SDL_PumpEvents();
		SDL_GetMouseState(&xpos, &ypos);
		return {(float)xpos, (float)ypos, 0};
	}

	void InputSDL::InitMappings()
	{
		Input::ClearKeyMap();
		Input::AddKeyTranslation(SDL_SCANCODE_ESCAPE, Key::Esc);
		Input::AddKeyTranslation(SDL_SCANCODE_RETURN, Key::Return);
		Input::AddKeyTranslation(SDL_SCANCODE_TAB, Key::Tab);
		Input::AddKeyTranslation(SDL_SCANCODE_BACKSPACE, Key::Backspace);
		Input::AddKeyTranslation(SDL_SCANCODE_SPACE, Key::Space);
		Input::AddKeyTranslation(SDL_SCANCODE_UP, Key::Up);
		Input::AddKeyTranslation(SDL_SCANCODE_DOWN, Key::Down);
		Input::AddKeyTranslation(SDL_SCANCODE_LEFT, Key::Left);
		Input::AddKeyTranslation(SDL_SCANCODE_RIGHT, Key::Right);
		Input::AddKeyTranslation(SDL_SCANCODE_PAGEUP, Key::PageUp);
		Input::AddKeyTranslation(SDL_SCANCODE_PAGEDOWN, Key::PageDown);
		Input::AddKeyTranslation(SDL_SCANCODE_HOME, Key::Home);
		Input::AddKeyTranslation(SDL_SCANCODE_END, Key::End);
		Input::AddKeyTranslation(SDL_SCANCODE_PRINTSCREEN, Key::Print);
		Input::AddKeyTranslation(SDL_SCANCODE_KP_PLUS, Key::Plus);
		Input::AddKeyTranslation(SDL_SCANCODE_EQUALS, Key::Plus);
		Input::AddKeyTranslation(SDL_SCANCODE_KP_MINUS, Key::Minus);
		Input::AddKeyTranslation(SDL_SCANCODE_MINUS, Key::Minus);
		Input::AddKeyTranslation(SDL_SCANCODE_GRAVE, Key::Tilde);
		Input::AddKeyTranslation(SDL_SCANCODE_KP_COMMA, Key::Comma);
		Input::AddKeyTranslation(SDL_SCANCODE_COMMA, Key::Comma);
		Input::AddKeyTranslation(SDL_SCANCODE_KP_PERIOD, Key::Period);
		Input::AddKeyTranslation(SDL_SCANCODE_PERIOD, Key::Period);
		Input::AddKeyTranslation(SDL_SCANCODE_SLASH, Key::Slash);
		Input::AddKeyTranslation(SDL_SCANCODE_F1, Key::F1);
		Input::AddKeyTranslation(SDL_SCANCODE_F2, Key::F2);
		Input::AddKeyTranslation(SDL_SCANCODE_F3, Key::F3);
		Input::AddKeyTranslation(SDL_SCANCODE_F4, Key::F4);
		Input::AddKeyTranslation(SDL_SCANCODE_F5, Key::F5);
		Input::AddKeyTranslation(SDL_SCANCODE_F6, Key::F6);
		Input::AddKeyTranslation(SDL_SCANCODE_F7, Key::F7);
		Input::AddKeyTranslation(SDL_SCANCODE_F8, Key::F8);
		Input::AddKeyTranslation(SDL_SCANCODE_F9, Key::F9);
		Input::AddKeyTranslation(SDL_SCANCODE_F10, Key::F10);
		Input::AddKeyTranslation(SDL_SCANCODE_F11, Key::F11);
		Input::AddKeyTranslation(SDL_SCANCODE_F12, Key::F12);
		Input::AddKeyTranslation(SDL_SCANCODE_KP_0, Key::NumPad0);
		Input::AddKeyTranslation(SDL_SCANCODE_KP_1, Key::NumPad1);
		Input::AddKeyTranslation(SDL_SCANCODE_KP_2, Key::NumPad2);
		Input::AddKeyTranslation(SDL_SCANCODE_KP_3, Key::NumPad3);
		Input::AddKeyTranslation(SDL_SCANCODE_KP_4, Key::NumPad4);
		Input::AddKeyTranslation(SDL_SCANCODE_KP_5, Key::NumPad5);
		Input::AddKeyTranslation(SDL_SCANCODE_KP_6, Key::NumPad6);
		Input::AddKeyTranslation(SDL_SCANCODE_KP_7, Key::NumPad7);
		Input::AddKeyTranslation(SDL_SCANCODE_KP_8, Key::NumPad8);
		Input::AddKeyTranslation(SDL_SCANCODE_KP_9, Key::NumPad9);
		Input::AddKeyTranslation(SDL_SCANCODE_KP_ENTER, Key::NumPadEnter);
		Input::AddKeyTranslation(SDL_SCANCODE_0, Key::Key0);
		Input::AddKeyTranslation(SDL_SCANCODE_1, Key::Key1);
		Input::AddKeyTranslation(SDL_SCANCODE_2, Key::Key2);
		Input::AddKeyTranslation(SDL_SCANCODE_3, Key::Key3);
		Input::AddKeyTranslation(SDL_SCANCODE_4, Key::Key4);
		Input::AddKeyTranslation(SDL_SCANCODE_5, Key::Key5);
		Input::AddKeyTranslation(SDL_SCANCODE_6, Key::Key6);
		Input::AddKeyTranslation(SDL_SCANCODE_7, Key::Key7);
		Input::AddKeyTranslation(SDL_SCANCODE_8, Key::Key8);
		Input::AddKeyTranslation(SDL_SCANCODE_9, Key::Key9);
		Input::AddKeyTranslation(SDL_SCANCODE_A, Key::KeyA);
		Input::AddKeyTranslation(SDL_SCANCODE_B, Key::KeyB);
		Input::AddKeyTranslation(SDL_SCANCODE_C, Key::KeyC);
		Input::AddKeyTranslation(SDL_SCANCODE_D, Key::KeyD);
		Input::AddKeyTranslation(SDL_SCANCODE_E, Key::KeyE);
		Input::AddKeyTranslation(SDL_SCANCODE_F, Key::KeyF);
		Input::AddKeyTranslation(SDL_SCANCODE_G, Key::KeyG);
		Input::AddKeyTranslation(SDL_SCANCODE_H, Key::KeyH);
		Input::AddKeyTranslation(SDL_SCANCODE_I, Key::KeyI);
		Input::AddKeyTranslation(SDL_SCANCODE_J, Key::KeyJ);
		Input::AddKeyTranslation(SDL_SCANCODE_K, Key::KeyK);
		Input::AddKeyTranslation(SDL_SCANCODE_L, Key::KeyL);
		Input::AddKeyTranslation(SDL_SCANCODE_M, Key::KeyM);
		Input::AddKeyTranslation(SDL_SCANCODE_N, Key::KeyN);
		Input::AddKeyTranslation(SDL_SCANCODE_O, Key::KeyO);
		Input::AddKeyTranslation(SDL_SCANCODE_P, Key::KeyP);
		Input::AddKeyTranslation(SDL_SCANCODE_Q, Key::KeyQ);
		Input::AddKeyTranslation(SDL_SCANCODE_R, Key::KeyR);
		Input::AddKeyTranslation(SDL_SCANCODE_S, Key::KeyS);
		Input::AddKeyTranslation(SDL_SCANCODE_T, Key::KeyT);
		Input::AddKeyTranslation(SDL_SCANCODE_U, Key::KeyU);
		Input::AddKeyTranslation(SDL_SCANCODE_V, Key::KeyV);
		Input::AddKeyTranslation(SDL_SCANCODE_W, Key::KeyW);
		Input::AddKeyTranslation(SDL_SCANCODE_X, Key::KeyX);
		Input::AddKeyTranslation(SDL_SCANCODE_Y, Key::KeyY);
		Input::AddKeyTranslation(SDL_SCANCODE_Z, Key::KeyZ);
	}

	uint8_t InputSDL::TranslateKeyModifiers(uint16_t key)
	{
		uint8_t modifiers = 0;
		modifiers |= key & KMOD_LALT ? Antomic::Modifier::LeftAlt : 0;
		modifiers |= key & KMOD_RALT ? Antomic::Modifier::RightAlt : 0;
		modifiers |= key & KMOD_LCTRL ? Antomic::Modifier::LeftCtrl : 0;
		modifiers |= key & KMOD_RCTRL ? Antomic::Modifier::RightCtrl : 0;
		modifiers |= key & KMOD_LSHIFT ? Antomic::Modifier::LeftShift : 0;
		modifiers |= key & KMOD_RSHIFT ? Antomic::Modifier::RightShift : 0;
		modifiers |= key & KMOD_LGUI ? Antomic::Modifier::LeftMeta : 0;
		modifiers |= key & KMOD_RGUI ? Antomic::Modifier::RightMeta : 0;
		return modifiers;
	}

	uint8_t InputSDL::TranslateKeyModifierPress(uint16_t key)
	{
		uint8_t modifier;
		switch(key)
		{
			case SDL_SCANCODE_LALT: {
				modifier = Antomic::Modifier::LeftAlt;
			}
			break;
			case SDL_SCANCODE_RALT: {
				modifier = Antomic::Modifier::RightAlt;
			}
			break;
			case SDL_SCANCODE_LCTRL: {
				modifier = Antomic::Modifier::LeftCtrl;
			}
			break;
			case SDL_SCANCODE_RCTRL: {
				modifier = Antomic::Modifier::RightCtrl;
			}
			break;
			case SDL_SCANCODE_LSHIFT: {
				modifier = Antomic::Modifier::LeftShift;
			}
			break;
			case SDL_SCANCODE_RSHIFT: {
				modifier = Antomic::Modifier::RightShift;
			}
			break;
			case SDL_SCANCODE_LGUI: {
				modifier = Antomic::Modifier::LeftMeta;
			}
			break;
			case SDL_SCANCODE_RGUI: {
				modifier = Antomic::Modifier::RightMeta;
			}
			break;
			default: {
				modifier = 0;
			}
			break;
		}

		return modifier;
	}
} // namespace Antomic
#endif