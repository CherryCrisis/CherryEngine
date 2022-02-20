
#include "panels/game_displayer.hpp"

#include <imgui.h>

void GameDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    if (ImGui::Begin("Game", &m_isOpened))
    {
        FrameDisplayer::Render();
        ImGui::BeginChild("GameFrameBuffer");
        ImVec2 wsize = ImGui::GetWindowSize();

        if (m_isActive)
            UpdateFramebuffer(wsize.x, wsize.y);

        ImGui::Image((ImTextureID)m_ViewTex, wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndChild();
    }

    ImGui::End();
}