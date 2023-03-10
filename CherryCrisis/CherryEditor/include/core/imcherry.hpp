#pragma once

#include <string>

#include <imgui.h>


namespace ImCherry 
{
	bool CheckboxBool3(const std::string& label, bool v[3]);
	bool Checkbox(const std::string& label, bool* v);
	bool ColoredDragFloat3(const std::string& label, float v[3], bool convert = false, float columnWidth = 80.f);
	bool ColoredButton(const char* label, ImVec4 color, ImVec2 size = {0,0}, bool isDisabled = false);
	bool DragFloat(const std::string& label, float* v, float v_speed, float v_min, float v_max, const char* format);
	void Dropzone(const std::string& label, const std::string& value);
	bool DropzoneMenu(const std::string& label, const std::string& value,const char* type, void** data, bool disabled = true);
	void CenteredText(const std::string& label);
	void SetKeyboardFocus();
}

namespace ImCherryInternal 
{
	void EndCherryComponent(int varPop=0, int colorPop=0);
	void BeginCherryComponent(const std::string& label);

	void BeginCherryHeader(bool active = true);
	void EndCherryHeader();
}

namespace ImGui 
{

}
