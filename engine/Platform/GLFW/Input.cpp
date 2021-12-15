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
#ifdef ANTOMIC_GLFW_PLATFORM
#	include "Platform/GLFW/Input.h"
#	include "GLFW/glfw3.h"
#	include "Platform/Platform.h"

namespace Antomic
{
	bool InputGLFW::IsKeyPressed(Key::Enum key)
	{
		auto* window = static_cast<GLFWwindow*>(Platform::GetWindow()->GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(Input::FromKey(key)));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool InputGLFW::IsMouseButtonPressed(MouseButton::Enum button)
	{
		auto* window = static_cast<GLFWwindow*>(Platform::GetWindow()->GetNativeWindow());

		int32_t aButton;

		switch(button)
		{
			default:
			case MouseButton::Left: aButton = GLFW_MOUSE_BUTTON_LEFT; break;
			case MouseButton::Middle: aButton = GLFW_MOUSE_BUTTON_MIDDLE; break;
			case MouseButton::Right: aButton = GLFW_MOUSE_BUTTON_RIGHT; break;
		}

		return glfwGetMouseButton(window, aButton) == GLFW_PRESS;
	}

	uint8_t InputGLFW::GetKeyModifiers() { return 0; }

	glm::vec3 InputGLFW::GetMousePosition()
	{
		auto* window = static_cast<GLFWwindow*>(Platform::GetWindow()->GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return {(float)xpos, (float)ypos, 0};
	}

	void InputGLFW::InitMappings()
	{
		Input::ClearKeyMap();
		Input::AddKeyTranslation(GLFW_KEY_ESCAPE, Key::Esc);
		Input::AddKeyTranslation(GLFW_KEY_ENTER, Key::Return);
		Input::AddKeyTranslation(GLFW_KEY_TAB, Key::Tab);
		Input::AddKeyTranslation(GLFW_KEY_BACKSPACE, Key::Backspace);
		Input::AddKeyTranslation(GLFW_KEY_SPACE, Key::Space);
		Input::AddKeyTranslation(GLFW_KEY_UP, Key::Up);
		Input::AddKeyTranslation(GLFW_KEY_DOWN, Key::Down);
		Input::AddKeyTranslation(GLFW_KEY_LEFT, Key::Left);
		Input::AddKeyTranslation(GLFW_KEY_RIGHT, Key::Right);
		Input::AddKeyTranslation(GLFW_KEY_PAGE_UP, Key::PageUp);
		Input::AddKeyTranslation(GLFW_KEY_PAGE_DOWN, Key::PageDown);
		Input::AddKeyTranslation(GLFW_KEY_HOME, Key::Home);
		Input::AddKeyTranslation(GLFW_KEY_END, Key::End);
		Input::AddKeyTranslation(GLFW_KEY_PRINT_SCREEN, Key::Print);
		Input::AddKeyTranslation(GLFW_KEY_KP_ADD, Key::Plus);
		Input::AddKeyTranslation(GLFW_KEY_EQUAL, Key::Plus);
		Input::AddKeyTranslation(GLFW_KEY_KP_SUBTRACT, Key::Minus);
		Input::AddKeyTranslation(GLFW_KEY_MINUS, Key::Minus);
		Input::AddKeyTranslation(GLFW_KEY_GRAVE_ACCENT, Key::Tilde);
		Input::AddKeyTranslation(GLFW_KEY_KP_DECIMAL, Key::Comma);
		Input::AddKeyTranslation(GLFW_KEY_COMMA, Key::Comma);
		Input::AddKeyTranslation(GLFW_KEY_PERIOD, Key::Period);
		Input::AddKeyTranslation(GLFW_KEY_SLASH, Key::Slash);
		Input::AddKeyTranslation(GLFW_KEY_F1, Key::F1);
		Input::AddKeyTranslation(GLFW_KEY_F2, Key::F2);
		Input::AddKeyTranslation(GLFW_KEY_F3, Key::F3);
		Input::AddKeyTranslation(GLFW_KEY_F4, Key::F4);
		Input::AddKeyTranslation(GLFW_KEY_F5, Key::F5);
		Input::AddKeyTranslation(GLFW_KEY_F6, Key::F6);
		Input::AddKeyTranslation(GLFW_KEY_F7, Key::F7);
		Input::AddKeyTranslation(GLFW_KEY_F8, Key::F8);
		Input::AddKeyTranslation(GLFW_KEY_F9, Key::F9);
		Input::AddKeyTranslation(GLFW_KEY_F10, Key::F10);
		Input::AddKeyTranslation(GLFW_KEY_F11, Key::F11);
		Input::AddKeyTranslation(GLFW_KEY_F12, Key::F12);
		Input::AddKeyTranslation(GLFW_KEY_KP_0, Key::NumPad0);
		Input::AddKeyTranslation(GLFW_KEY_KP_1, Key::NumPad1);
		Input::AddKeyTranslation(GLFW_KEY_KP_2, Key::NumPad2);
		Input::AddKeyTranslation(GLFW_KEY_KP_3, Key::NumPad3);
		Input::AddKeyTranslation(GLFW_KEY_KP_4, Key::NumPad4);
		Input::AddKeyTranslation(GLFW_KEY_KP_5, Key::NumPad5);
		Input::AddKeyTranslation(GLFW_KEY_KP_6, Key::NumPad6);
		Input::AddKeyTranslation(GLFW_KEY_KP_7, Key::NumPad7);
		Input::AddKeyTranslation(GLFW_KEY_KP_8, Key::NumPad8);
		Input::AddKeyTranslation(GLFW_KEY_KP_9, Key::NumPad9);
		Input::AddKeyTranslation(GLFW_KEY_KP_ENTER, Key::NumPadEnter);
		Input::AddKeyTranslation(GLFW_KEY_0, Key::Key0);
		Input::AddKeyTranslation(GLFW_KEY_1, Key::Key1);
		Input::AddKeyTranslation(GLFW_KEY_2, Key::Key2);
		Input::AddKeyTranslation(GLFW_KEY_3, Key::Key3);
		Input::AddKeyTranslation(GLFW_KEY_4, Key::Key4);
		Input::AddKeyTranslation(GLFW_KEY_5, Key::Key5);
		Input::AddKeyTranslation(GLFW_KEY_6, Key::Key6);
		Input::AddKeyTranslation(GLFW_KEY_7, Key::Key7);
		Input::AddKeyTranslation(GLFW_KEY_8, Key::Key8);
		Input::AddKeyTranslation(GLFW_KEY_9, Key::Key9);
		Input::AddKeyTranslation(GLFW_KEY_A, Key::KeyA);
		Input::AddKeyTranslation(GLFW_KEY_B, Key::KeyB);
		Input::AddKeyTranslation(GLFW_KEY_C, Key::KeyC);
		Input::AddKeyTranslation(GLFW_KEY_D, Key::KeyD);
		Input::AddKeyTranslation(GLFW_KEY_E, Key::KeyE);
		Input::AddKeyTranslation(GLFW_KEY_F, Key::KeyF);
		Input::AddKeyTranslation(GLFW_KEY_G, Key::KeyG);
		Input::AddKeyTranslation(GLFW_KEY_H, Key::KeyH);
		Input::AddKeyTranslation(GLFW_KEY_I, Key::KeyI);
		Input::AddKeyTranslation(GLFW_KEY_J, Key::KeyJ);
		Input::AddKeyTranslation(GLFW_KEY_K, Key::KeyK);
		Input::AddKeyTranslation(GLFW_KEY_L, Key::KeyL);
		Input::AddKeyTranslation(GLFW_KEY_M, Key::KeyM);
		Input::AddKeyTranslation(GLFW_KEY_N, Key::KeyN);
		Input::AddKeyTranslation(GLFW_KEY_O, Key::KeyO);
		Input::AddKeyTranslation(GLFW_KEY_P, Key::KeyP);
		Input::AddKeyTranslation(GLFW_KEY_Q, Key::KeyQ);
		Input::AddKeyTranslation(GLFW_KEY_R, Key::KeyR);
		Input::AddKeyTranslation(GLFW_KEY_S, Key::KeyS);
		Input::AddKeyTranslation(GLFW_KEY_T, Key::KeyT);
		Input::AddKeyTranslation(GLFW_KEY_U, Key::KeyU);
		Input::AddKeyTranslation(GLFW_KEY_V, Key::KeyV);
		Input::AddKeyTranslation(GLFW_KEY_W, Key::KeyW);
		Input::AddKeyTranslation(GLFW_KEY_X, Key::KeyX);
		Input::AddKeyTranslation(GLFW_KEY_Y, Key::KeyY);
		Input::AddKeyTranslation(GLFW_KEY_Z, Key::KeyZ);
	}

} // namespace Antomic
#endif