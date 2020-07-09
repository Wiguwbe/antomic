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
#include "ImGUI.h"
#include "imgui.h"

namespace Editor
{

  ImGUILayer::ImGUILayer() : Engine::Layer("ImGUILayer")
  {
  }

  ImGUILayer::~ImGUILayer()
  {
  }

  void ImGUILayer::OnAttach()
  {
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_::ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_::ImGuiBackendFlags_HasSetMousePos;
    ImGui_Implbgfx_Init(255);
  }

  void ImGUILayer::OnDetach()
  {
    ImGui_Implbgfx_Shutdown();
    ImGui::DestroyContext();
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
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(
        (float)Engine::Application::Current().CurrentWidth(),
        (float)Engine::Application::Current().CurrentHeight());

    io.DeltaTime = 1 / 60.f;
    ImGui_Implbgfx_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow(); // your drawing here
    ImGui::Render();
  }

  bool ImGUILayer::OnMouseMovedEvent(Engine::MouseMovedEvent &event)
  {
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2(event.GetX(),event.GetY());
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