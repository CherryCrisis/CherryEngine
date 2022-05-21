#include "core/imcherry.hpp"

#include <string>
#include <imgui_internal.h>


namespace ImCherry
{
	// Code borrowed to THECHERNO on his engine serie
	bool ColoredDragFloat3(const std::string& label, float v[3], float columnWidth)
	{
		ImGui::PushID(label.c_str());
		bool value = false;

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,0 });

		float height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { height + 3.f, height };

		ColoredButton("X", { .8f, .15f, .15f, 1.f }, buttonSize, true);

		ImGui::SameLine();
		value |= ImGui::DragFloat("##X", &v[0], 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ColoredButton("Y", { .15f, .8f, .15f, 1.f }, buttonSize, true);

		ImGui::SameLine();
		value |= ImGui::DragFloat("##Y", &v[1], 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ColoredButton("Z", { .15f, .15f, .8f, 1.f }, buttonSize, true);

		ImGui::SameLine();
		value |= ImGui::DragFloat("##Z", &v[2], 0.1f);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();

		return value;
	}

	bool ColoredButton(const char* label, ImVec4 color, ImVec2 size, bool isDisabled) 
	{
		ImGui::PushStyleColor(ImGuiCol_Button, color);
		if (isDisabled) ImGui::BeginDisabled();
		bool value = ImGui::Button(label, size);
		if (isDisabled) ImGui::EndDisabled();
		ImGui::PopStyleColor();

		return value;
	}
}