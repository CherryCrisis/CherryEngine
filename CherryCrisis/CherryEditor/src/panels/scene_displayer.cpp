
#include "panels/scene_displayer.hpp"

#include <imgui.h>

void SceneDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    m_isActive = true;

    if (ImGui::Begin("Scene", &m_isOpened))
    {
        m_isActive = !ImGui::IsWindowCollapsed();

        ImGui::BeginChild("SceneFrameBuffer");
        ImVec2 wsize = ImGui::GetWindowSize();
        
        if (m_isActive)
            UpdateFramebuffer(wsize.x, wsize.y);

        uint64_t ViewTex = (uint64_t)m_ViewTex;

        ImGui::Image((ImTextureID)ViewTex, wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndChild();
    }

    ImGui::End();
}