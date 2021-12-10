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
#include "Core/LayerStack.h"
#include "Platform/Input.h"
#include "Platform/Platform.h"
#include "glm/glm.hpp"

namespace Antomic
{
	class Application
	{

	public:
		Application()
			: Application("Application", 640, 480){};
		Application(const std::string& title)
			: Application(title, 640, 480){};
		Application(const std::string& title,
					uint32_t width,
					uint32_t height,
					RenderAPIDialect api = RenderAPIDialect::OPENGL);
		virtual ~Application() = default;

	public:
		// Control Operations
		void ToggleFullscreen(bool value);
		void Run();

		// Windows attributes
		inline uint32_t GetWidth() const
		{
			return Platform::GetWindowWidth();
		}
		inline uint32_t GetHeight() const
		{
			return Platform::GetWindowHeight();
		}

		// Events
		void OnEvent(Event& event);

		// Windows Events
		virtual bool OnWindowClose(WindowCloseEvent& event);
		virtual bool OnWindowResize(WindowResizeEvent& event);

		// Keys Events
		virtual bool OnKeyPressed(KeyPressedEvent& event)
		{
			return true;
		}
		virtual bool OnKeyReleased(KeyReleasedEvent& event)
		{
			return true;
		}

		// Mouse Events
		virtual bool OnMouseMoved(MouseMovedEvent& event)
		{
			return true;
		}
		virtual bool OnMouseScrolled(MouseScrolledEvent& event)
		{
			return true;
		}
		virtual bool OnMouseButtonPressed(MouseButtonPressedEvent& event)
		{
			return true;
		}
		virtual bool OnMouseButtonReleased(MouseButtonReleasedEvent& event)
		{
			return true;
		}

		// Scene
		void SetScene(const Ref<Scene>& scene);
		void LoadScene(const std::string& name);

	public:
		static Application& Current()
		{
			return *sInstance;
		}

	private:
		static Application* sInstance;
		bool mRunning;
		glm::mat4 mProjMatrix;
		Ref<Renderer> mRenderer;
	};
} // namespace Antomic
