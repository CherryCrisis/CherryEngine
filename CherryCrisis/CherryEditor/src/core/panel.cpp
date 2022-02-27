
#include "core/panel.hpp"

#include <imgui.h>

bool PanelCategory::DisplayLabel()
{
	return ImGui::MenuItem(m_name.c_str());
}

void PanelCategory::DisplayCategory()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    
    ImGui::BeginChild("CategoryFocus", ImVec2(0, ImGui::GetContentRegionAvail().y), true, window_flags);

    // Need to properly set it to the middle (rn start of the text is in the middle)
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2.f );
    ImGui::Text(m_name.c_str());

    Fill();
    /*
    if (ImGui::BeginTable("split", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
    {

        ImGui::EndTable();
    }
    */
    ImGui::EndChild();
    ImGui::PopStyleVar();
}