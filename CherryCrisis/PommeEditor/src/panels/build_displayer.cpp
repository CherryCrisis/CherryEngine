
#include "panels/build_displayer.hpp"

#include <imgui.h>

void BuildDisplayer::Render() 
{
	if (!m_isOpened)
		return;

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(.5f, .5f));
    if (ImGui::Begin("Builder", &m_isOpened))
    {
        if (ImGui::Button("Close")) { m_isOpened = false; }
        ImGui::SameLine();
        if (ImGui::Button("Build")) {}

        ImGui::End();
    }
}