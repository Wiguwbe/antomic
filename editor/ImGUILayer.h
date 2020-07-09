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
#include "engine.h"
#include "ImGUIRenderer.h"
#include "imgui.h"

namespace Editor
{

    class ImGUILayer : public Engine::Layer
    {
    private:
    public:
        ImGUILayer();
        ~ImGUILayer();

        void OnAttach() override;
        void OnDetach() override;

        void OnEvent(Engine::Event &e) override;
        void Update() override;
        void Render() override;

        bool OnMouseMovedEvent(Engine::MouseMovedEvent &event);
        bool OnMousePressed(Engine::MouseButtonPressedEvent &event);
        bool OnMouseReleased(Engine::MouseButtonReleasedEvent &event);

    private:
        ImGuiContext* m_Context;
        bgfx::TextureHandle m_FontTexture;
    };
} // namespace Editor