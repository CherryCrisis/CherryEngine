
#include "panels/preferences_displayer.hpp"

#include <imgui.h>

void PreferencesDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(.5f, .5f));
    if (ImGui::Begin("Preferences", &m_isOpened)) 
    {
        if (ImGui::Button("Close")) { m_isOpened = false; }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {}
        ImGui::End();
    }
}