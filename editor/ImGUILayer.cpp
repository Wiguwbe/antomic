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
#include "ImGUILayer.h"

namespace Editor
{

    ImGUILayer::ImGUILayer() : Engine::Layer("ImGUILayer"), m_Context(nullptr), m_FontTexture(BGFX_INVALID_HANDLE)
    {
    }

    ImGUILayer::~ImGUILayer()
    {
    }

    void ImGUILayer::OnAttach()
    {
        m_Context = ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_::ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_::ImGuiBackendFlags_HasSetMousePos;

        io.KeyMap[ImGuiKey_Tab] = Engine::Key::Enum::Tab;
        io.KeyMap[ImGuiKey_LeftArrow] = Engine::Key::Enum::Left;
        io.KeyMap[ImGuiKey_RightArrow] = Engine::Key::Enum::Right;
        io.KeyMap[ImGuiKey_UpArrow] = Engine::Key::Enum::Up;
        io.KeyMap[ImGuiKey_DownArrow] = Engine::Key::Enum::Down;
        io.KeyMap[ImGuiKey_PageUp] = Engine::Key::Enum::PageUp;
        io.KeyMap[ImGuiKey_PageDown] = Engine::Key::Enum::PageDown;
        io.KeyMap[ImGuiKey_Home] = Engine::Key::Enum::Home;
        io.KeyMap[ImGuiKey_End] = Engine::Key::Enum::Tab;
        io.KeyMap[ImGuiKey_Insert] = Engine::Key::Enum::Insert;
        io.KeyMap[ImGuiKey_Delete] = Engine::Key::Enum::Delete;
        io.KeyMap[ImGuiKey_Backspace] = Engine::Key::Enum::Backspace;
        io.KeyMap[ImGuiKey_Space] = Engine::Key::Enum::Space;
        io.KeyMap[ImGuiKey_Enter] = Engine::Key::Enum::Return;
        io.KeyMap[ImGuiKey_Escape] = Engine::Key::Enum::Esc;
        io.KeyMap[ImGuiKey_KeyPadEnter] = Engine::Key::Enum::NumPadEnter;
        io.KeyMap[ImGuiKey_A] = Engine::Key::Enum::KeyA;
        io.KeyMap[ImGuiKey_C] = Engine::Key::Enum::KeyC;
        io.KeyMap[ImGuiKey_V] = Engine::Key::Enum::KeyV;
        io.KeyMap[ImGuiKey_X] = Engine::Key::Enum::KeyX;
        io.KeyMap[ImGuiKey_Y] = Engine::Key::Enum::KeyY;
        io.KeyMap[ImGuiKey_Z] = Engine::Key::Enum::KeyZ;
        io.RenderDrawListsFn = ImGUIRenderer::RenderDrawLists;

        // Build texture atlas
        unsigned char *pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        io.Fonts->TexID = ImGUIRenderer::GetTextureId(width, height, pixels, m_FontTexture);
    }

    void ImGUILayer::OnDetach()
    {
        if (!isValid(m_FontTexture))
            return;

        ImGui::SetCurrentContext(m_Context);
        ImGuiIO &io = ImGui::GetIO();
        io.Fonts->TexID = nullptr;
        m_FontTexture.idx = BGFX_INVALID_HANDLE;

        bgfx::destroy(m_FontTexture);
        ImGui::DestroyContext(m_Context);
    }

    void ImGUILayer::OnEvent(Engine::Event &e)
    {
        Engine::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<Engine::MouseMovedEvent>(ENGINE_BIND_EVENT_FN(ImGUILayer::OnMouseMovedEvent));
        dispatcher.Dispatch<Engine::MouseButtonPressedEvent>(ENGINE_BIND_EVENT_FN(ImGUILayer::OnMousePressed));
        dispatcher.Dispatch<Engine::MouseButtonReleasedEvent>(ENGINE_BIND_EVENT_FN(ImGUILayer::OnMouseReleased));
    }

    void ImGUILayer::Update()
    {
    }

    void ImGUILayer::Render()
    {
        ImGui::SetCurrentContext(m_Context);
        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize = ImVec2(
                (float)Engine::Application::Current().CurrentWidth(),
                (float)Engine::Application::Current().CurrentHeight());

        io.DeltaTime = 1 / 60.f;
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::Render();
    }

    bool ImGUILayer::OnMouseMovedEvent(Engine::MouseMovedEvent &event)
    {
        ImGuiIO &io = ImGui::GetIO();
        io.MousePos = ImVec2(event.GetX(), event.GetY());
        return true;
    }

    bool ImGUILayer::OnMousePressed(Engine::MouseButtonPressedEvent &event)
    {
        ImGuiIO &io = ImGui::GetIO();
        io.MouseDown[event.GetMouseButton()] = true;
        return true;
    }

    bool ImGUILayer::OnMouseReleased(Engine::MouseButtonReleasedEvent &event)
    {
        ImGuiIO &io = ImGui::GetIO();
        io.MouseDown[event.GetMouseButton()] = false;
        return true;
    }

} // namespace Editor