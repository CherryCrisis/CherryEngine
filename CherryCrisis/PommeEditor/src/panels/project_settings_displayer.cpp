
#include "panels/project_settings_displayer.hpp"

#include <imgui.h>

void ProjectSettingsDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(.5f, .5f));
    if (ImGui::Begin("Project Settings", &m_isOpened))
    {

        // Add a bunch of stuff



        ImGui::Columns(2, "Categories");
        if (ImGui::Button("Inputs")) {}
        ImGui::NextColumn();
        ImGui::TextUnformatted("YE");
        ImGui::Columns();



        
        if (ImGui::Button("Close")) { m_isOpened = false; }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {}

        ImGui::End();
    }
}