
#include "panels/build_displayer.hpp"

#include <imgui.h>

#include "builder.hpp"
#include "portable-file-dialogs.hpp"
#undef far
#undef near
#undef ERROR
#include "core/editor_manager.hpp"
#include "panels/project_settings_displayer.hpp"

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

void BuildDisplayer::Render() 
{
	if (!m_isOpened)
		return;

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(.5f, .5f));
    if (ImGui::Begin("Builder", &m_isOpened))
    {
        IMGUI_LEFT_LABEL(ImGui::InputText, "Build Directory :", outDir, IM_ARRAYSIZE(outDir));
        ImGui::SameLine(); if (ImGui::Button("...")) 
        {
            auto selection = pfd::select_folder("Select a folder").result();
            const char* cSelection = selection.c_str();

            if (!selection.empty())
                memcpy(outDir, cSelection, sizeof(char) * strlen(cSelection));
        }

        if (ImGui::Button("Close")) { m_isOpened = false; }
        ImGui::SameLine();
        if (ImGui::Button("Build")) 
        {
            bool result = Builder::BuildGame(outDir, projectSettings->GetBuildSettings().m_gameName.c_str());
            EditorNotifications::BuildGame(result);
            if (result)
                m_isOpened = false;
        }
    }
    ImGui::End();
}