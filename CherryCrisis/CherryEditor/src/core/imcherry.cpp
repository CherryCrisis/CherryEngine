#include "core/imcherry.hpp"

#include <string>
#include <imgui_internal.h>


namespace ImCherry 
{
	bool ColoredDragFloat3(const char* label, float v[3], ImVec4 color, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
	{
		bool value = false;
		ImGui::PushStyleColor(ImGuiCol_Border, color);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
		if (ImGui::BeginTable("###DragTable", 2, ImGuiTableFlags_SizingStretchProp)) 
		{
			ImGui::TableSetupColumn(0, ImGuiTableColumnFlags_NoResize, .2f);
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(label);
			ImGui::TableSetColumnIndex(1);
			value = ImGui::DragFloat3("###", v, v_speed, v_min, v_max, format, flags);
			ImGui::EndTable();
		}
		ImGui::PopStyleColor(1);
		ImGui::PopStyleVar(1);
		return value;
	}

	bool ColoredDragFloat(const char* label, float* v, ImVec4 color, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags) 
	{
		ImGui::PushStyleColor(ImGuiCol_Border, color);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
		//ImGui::Text(label); ImGui::SameLine();
		bool value = ImGui::DragFloat(std::string(("###")+std::string(label)).c_str(), v, v_speed, v_min, v_max, format, flags);
		ImGui::PopStyleColor(1);
		ImGui::PopStyleVar(1);
		return value;
	}


	bool ColoredDragFloat3(const char* label, float v[3], ImVec4 color1, ImVec4 color2, ImVec4 color3, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
	{
		bool value = false;
		if (ImGui::BeginTable("###DragTable", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn(0, 0, .25f);
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(label);
			ImGui::TableSetColumnIndex(1);
			ImGui::PushMultiItemsWidths(3, ImGui::GetContentRegionAvail().x - 70);
			{
				value  = ColoredDragFloat("X", &v[0], color1, v_speed, v_min, v_max, format, flags);
				ImGui::PopItemWidth();
				ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x + 10);
				value |= ColoredDragFloat("Y", &v[1], color2, v_speed, v_min, v_max, format, flags);
				ImGui::PopItemWidth();
				ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x + 10);
				value |= ColoredDragFloat("Z", &v[2], color3, v_speed, v_min, v_max, format, flags);
				ImGui::PopItemWidth();
			}
			ImGui::EndTable();
		}


		return value;
	}
}