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
#	include "Platform/GLFW/Window.h"
#	include "Core/Log.h"
#	include "Events/KeyEvent.h"
#	include "Events/MouseEvent.h"
#	include "Events/WindowEvent.h"

namespace Antomic
{
	GLFWWindow::GLFWWindow(uint32_t width, uint32_t height, std::string title, RenderAPIDialect api)
	{
		mWindow = nullptr;

		if(!glfwInit())
		{
			ANTOMIC_TRACE("GLFWWindow: Error initializing GLFW");
			return;
		}

		ANTOMIC_INFO("GLFWWindow: Creating window {0},{1} with OpenGL support", width, height);
		mWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
		if(!mWindow)
		{
			ANTOMIC_TRACE("GLFWWindow: Error creating SDL window: {0},{1}", width, height);
			glfwTerminate();
			return;
		}

#	ifdef ANTOMIC_GL_RENDERER
		glfwMakeContextCurrent(mWindow);

		if(!gladLoadGL())
		{
			ANTOMIC_TRACE("GLFWWindow: Error initializing Glad");
			glfwDestroyWindow(mWindow);
			glfwTerminate();
			mWindow = nullptr;
			return;
		}

		ANTOMIC_INFO("GLFWWindow: Using OpenGL version {0}", glGetString(GL_VERSION));
#	endif

		// Initialize mappings from glfw -> antomic
		glfwSetWindowUserPointer(mWindow, &mData);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;
			WindowResizeEvent event(width, height);
			data.Handler(event);
		});

		glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.Handler(event);
		});

		glfwSetKeyCallback(mWindow,
						   [](GLFWwindow* window, int key, int scancode, int action, int mods) {
							   WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

							   switch(action)
							   {
								   case GLFW_PRESS: {
									   KeyPressedEvent event(Input::ToKey(key), 0, 0);
									   data.Handler(event);
									   break;
								   }
								   case GLFW_RELEASE: {
									   KeyReleasedEvent event(Input::ToKey(key), 0);
									   data.Handler(event);
									   break;
								   }
								   case GLFW_REPEAT: {
									   KeyPressedEvent event(Input::ToKey(key), 0, 1);
									   data.Handler(event);
									   break;
								   }
							   }
						   });

		glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned int keycode) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(Input::ToKey(keycode), 0);
			data.Handler(event);
		});

		glfwSetMouseButtonCallback(
			mWindow, [](GLFWwindow* window, int button, int action, int mods) {
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseButton::Enum aButton;

				switch(button)
				{
					default:
					case GLFW_MOUSE_BUTTON_LEFT: aButton = MouseButton::Left; break;
					case GLFW_MOUSE_BUTTON_MIDDLE: aButton = MouseButton::Middle; break;
					case GLFW_MOUSE_BUTTON_RIGHT: aButton = MouseButton::Right; break;
				}

				switch(action)
				{
					case GLFW_PRESS: {
						MouseButtonPressedEvent event(aButton);
						data.Handler(event);
						break;
					}
					case GLFW_RELEASE: {
						MouseButtonReleasedEvent event(aButton);
						data.Handler(event);
						break;
					}
				}
			});

		glfwSetScrollCallback(mWindow, [](GLFWwindow* window, double xOffset, double yOffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.Handler(event);
		});

		glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xPos, double yPos) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.Handler(event);
		});

		mData.Width = width;
		mData.Height = height;
		mData.Title = title;
	}

	GLFWWindow::~GLFWWindow()
	{
		if(mWindow)
		{
			glfwDestroyWindow(mWindow);
			mWindow = nullptr;
		}
		glfwTerminate();
	}

	void GLFWWindow::SetTitle(const std::string& title)
	{
		if(mWindow)
		{
			glfwSetWindowTitle(mWindow, title.c_str());
		}
		mData.Title = title;
	}

	void GLFWWindow::SwapBuffer() { glfwSwapBuffers(mWindow); }

	void GLFWWindow::ProcessEvents() { glfwPollEvents(); }

	void GLFWWindow::ToggleFullscreen() { }

	void GLFWWindow::SetMouseLock(bool lock) { }

	void GLFWWindow::Close()
	{
		if(mWindow)
		{
			glfwDestroyWindow(mWindow);
			mWindow = nullptr;
		}
		glfwTerminate();
	}
} // namespace Antomic
#endif