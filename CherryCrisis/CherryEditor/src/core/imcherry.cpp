#include "core/imcherry.hpp"

#include <string>

#include <imgui_internal.h>

#include "maths.hpp"

namespace ImCherry
{
	// Code borrowed to THECHERNO on his engine serie
	bool ColoredDragFloat3(const std::string& label, float v[3], bool convert, float columnWidth)
	{
		CCMaths::Vector3 toDeg = { CCMaths::ToDegrees(v[0]), CCMaths::ToDegrees(v[1]), CCMaths::ToDegrees(v[2]) };

		ImGui::PushID(label.c_str());
		bool value = false;
		
		ImCherryInternal::BeginCherryComponent(label);

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,0 });

		float height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { height + 3.f, height };

		ColoredButton("X", { .8f, .15f, .15f, 1.f }, buttonSize, true);

		ImGui::SameLine();
		if (convert)
			value |= ImGui::DragFloat("##X", &toDeg.x, 0.1f);
		else
			value |= ImGui::DragFloat("##X", &v[0], 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ColoredButton("Y", { .15f, .8f, .15f, 1.f }, buttonSize, true);

		ImGui::SameLine();
		if (convert)
			value |= ImGui::DragFloat("##Y", &toDeg.y, 0.1f);
		else
			value |= ImGui::DragFloat("##Y", &v[1], 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ColoredButton("Z", { .15f, .15f, .8f, 1.f }, buttonSize, true);

		ImGui::SameLine();
		if (convert)
			value |= ImGui::DragFloat("##Z", &toDeg.z, 0.1f);
		else
			value |= ImGui::DragFloat("##Z", &v[2], 0.1f);
		ImGui::PopItemWidth();

		ImCherryInternal::EndCherryComponent(1);

		if (convert) 
		{
			v[0] = CCMaths::ToRadians(toDeg.x);
			v[1] = CCMaths::ToRadians(toDeg.y);
			v[2] = CCMaths::ToRadians(toDeg.z);
		}

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

	bool Checkbox(const std::string& label, bool* v)
	{
		ImGui::PushID(label.c_str());
		bool value = false;

		ImCherryInternal::BeginCherryComponent(label);

		value = ImGui::Checkbox("###", v);

		ImCherryInternal::EndCherryComponent();
		return value;
	}

	bool CheckboxBool3(const std::string& label, bool v[3])
	{
		ImGui::PushID(label.c_str());
		bool value = false;

		ImCherryInternal::BeginCherryComponent(label);

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,0 });

		float height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { height + 3.f, height };

		ColoredButton("X", { .8f, .15f, .15f, 1.f }, buttonSize, true);

		ImGui::SameLine();
		value |= ImGui::Checkbox("##X", &v[0]);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ColoredButton("Y", { .15f, .8f, .15f, 1.f }, buttonSize, true);

		ImGui::SameLine();
		value |= ImGui::Checkbox("##Y", &v[1]);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ColoredButton("Z", { .15f, .15f, .8f, 1.f }, buttonSize, true);

		ImGui::SameLine();
		value |= ImGui::Checkbox("##Z", &v[2]);
		ImGui::PopItemWidth();

		ImCherryInternal::EndCherryComponent(1);

		return value;
	}

	bool DragFloat(const std::string& label, float* v, float v_speed, float v_min, float v_max, const char* format)
	{
		ImGui::PushID(label.c_str());
		bool value = false;

		ImCherryInternal::BeginCherryComponent(label);

		value = ImGui::DragFloat("###", v, v_speed, v_min, v_max, format);

		ImCherryInternal::EndCherryComponent();
		return value;
	}

	void Dropzone(const std::string& label, const std::string& value)
	{
		ImGui::PushID(label.c_str());
		ImCherryInternal::BeginCherryComponent(label);

		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
		ImGui::PushStyleColor(ImGuiCol_Border, { .75f, .75f, .75f, 1.000f });
		ImGui::BeginDisabled();
		ImGui::InputText("###value", (char*)value.c_str(), value.size());
		ImGui::EndDisabled();

		ImCherryInternal::EndCherryComponent(1, 1);
	}

	bool DropzoneMenu(const std::string& label, const std::string& value, bool disabled)
	{
		bool val = false;

		ImGui::PushID(label.c_str());
		ImCherryInternal::BeginCherryComponent(label);

		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
		ImGui::PushStyleColor(ImGuiCol_Border, {   .75f,   .75f,   .75f, 1.000f });
		if (disabled) ImGui::BeginDisabled();

		val = ImGui::Button(value.c_str());

		ImGui::SameLine();

		//-- TODO --------------------------------------------
		ImGui::BeginDisabled();
		if (ImGui::Button("..."))
		{
			//Open assets node window with extension match
		}
		ImGui::EndDisabled();
		//----------------------------------------------------

		if (disabled) ImGui::EndDisabled();

		ImCherryInternal::EndCherryComponent(1, 4);
		return val;
	}

	void SetKeyboardFocus()
	{
		if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
			ImGui::SetKeyboardFocusHere(0);
	}
}

namespace ImCherryInternal
{
	void BeginCherryComponent(const std::string& label)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 5.f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 0.f, 0.f });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 7.f, 7.f });
		ImGui::PushStyleColor(ImGuiCol_Separator, { 0.f, 0.f, 0.f, 0.f });
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 80.f);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();
		ImGui::PopStyleVar(3);
	}

	void EndCherryComponent(int varPop, int colorPop)
	{
		// separator color poped constantly
		ImGui::PopStyleColor(1 + colorPop);
		ImGui::PopStyleVar(varPop);
		ImGui::Columns(1);
		ImGui::PopID();
	}

	void BeginCherryHeader()
	{
		ImGui::PushStyleColor(ImGuiCol_Header, { 0.166f, 0.166f, 0.221f, 1.000f });
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 0.427f, 0.427f, 0.456f, 1.000f });
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, { 0.220f, 0.205f, 0.235f, 1.000f });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.f);
	}

	void EndCherryHeader()
	{
		ImGui::PopStyleVar(1);
		ImGui::PopStyleColor(3);
	}
}