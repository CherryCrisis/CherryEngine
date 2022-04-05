
#include "panels/game_displayer.hpp"

#include <imgui.h>

#include "input_manager.hpp"

void GameDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    m_isHovered = false;

    if (ImGui::Begin("Game", &m_isOpened))
    {
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
            m_isHovered = true;

        m_isActive = !ImGui::IsWindowCollapsed();

        ImGui::SetWindowSize({1920,1080});

        FrameDisplayer::Render();
        ImGui::BeginChild("GameFrameBuffer");
        ImVec2 wsize = ImGui::GetWindowSize();

        if (m_isActive)
            UpdateFramebuffer(wsize.x, wsize.y);

        uint64_t ViewTex = (uint64_t)m_ViewTex;

        ImGui::Image((ImTextureID)ViewTex, wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndChild();
    }

    ImGui::End();
}
