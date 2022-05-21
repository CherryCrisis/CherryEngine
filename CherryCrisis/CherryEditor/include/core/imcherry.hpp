#pragma once

#include "imgui.h"
#include <string>

namespace ImCherry 
{
	bool ColoredDragFloat3(const std::string& label, float v[3], float columnWidth=100.f);
	bool ColoredDragFloat(const char* label, float* v, ImVec4 color, float v_speed = 1.f, float v_min = 0.f, float v_max = 0.f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
	bool ColoredButton(const char* label, ImVec4 color, ImVec2 size = {0,0}, bool isDisabled = false);
}

namespace ImGui 
{

}