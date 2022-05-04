
#include "panels/game_displayer.hpp"

#include <imgui.h>

#include "input_manager.hpp"

#include "scene_manager.hpp"
#include "camera_component.hpp"

void GameDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    m_isHovered = false;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f,0.f });
    if (ImGui::Begin("Game", &m_isOpened))
    {
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
            m_isHovered = true;

        m_isActive = !ImGui::IsWindowCollapsed();

        ImGui::SetWindowSize({1920,1080});

        FrameDisplayer::Render();
        ImGui::BeginChild("GameFrameBuffer");
        ImVec2 wsize = ImGui::GetWindowSize();

        if (m_isActive && CameraComponent::GetMainCamera())
            if (Camera* cam = &CameraComponent::GetMainCamera()->m_camera)
                UpdateFramebuffer(wsize.x, wsize.y, *cam);

        uint64_t ViewTex = (uint64_t)m_framebuffer.colorTex.texID;

        ImGui::Image((ImTextureID)ViewTex, wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndChild();
    }
     
    ImGui::PopStyleVar(1);
    ImGui::End();
}

void GameDisplayer::Focus()
{
    m_inputs->SetPollContext("User Context");
    m_inputs->SetCursorHidden();
    m_isFocused = true;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    ImGui::SetWindowFocus("Game");
}

void GameDisplayer::Unfocus()
{
    m_inputs->SetPollContext(nullptr);
    m_inputs->SetCursorDisplayed();
    m_isFocused = false;
    ImGui::GetIO().ConfigFlags = ImGui::GetIO().ConfigFlags & ~ImGuiConfigFlags_NoMouse;
}