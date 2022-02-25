
#include "panels/project_settings_displayer.hpp"

#include <imgui.h>

ProjectSettingsDisplayer::ProjectSettingsDisplayer(bool spawnOpened = true) : Panel(spawnOpened) 
{
    m_categories[0] = "Generals";
    m_categories[1] = "Audio";
    m_categories[2] = "Physics";
    m_categories[3] = "Inputs";
    m_categories[4] = "Tags / Layers";
    m_categories[5] = "Render Pass";
}

void ProjectSettingsDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(.5f, .5f));

    if (ImGui::Begin("Project Settings", &m_isOpened, ImGuiWindowFlags_MenuBar))
    {
        ImGui::BeginMenuBar();
        static char name[32] = "";
        ImGui::InputText("Search", name, IM_ARRAYSIZE(name));
        ImGui::EndMenuBar();

        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
            ImGui::BeginChild("CategoryList", ImVec2(ImGui::GetContentRegionAvail().x * 0.25f, ImGui::GetContentRegionAvail().y), false, window_flags);
            for (const std::string& name : m_categories ) 
            {
                ImGui::Text(name.c_str());
            }

            ImGui::EndChild();
        }

        ImGui::SameLine();

        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild("CategoryFocus", ImVec2(0, ImGui::GetContentRegionAvail().y), true, window_flags);
            if (ImGui::BeginTable("split", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
            {
                for (int i = 0; i < 100; i++)
                {
                    ImGui::Text("Key");
                    ImGui::Button("value", ImVec2(-FLT_MIN, 0.0f));
                    ImGui::TableNextColumn();
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();
        }


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