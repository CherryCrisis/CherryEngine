#pragma once

#include "imgui.h"

namespace ImCherry 
{
	bool ColoredDragFloat3(const char* label, float v[3], ImVec4 color, float v_speed=1.f, float v_min=0.f, float v_max=0.f, const char* format="%.3f", ImGuiSliderFlags flags=0);
	bool ColoredDragFloat3(const char* label, float v[3], ImVec4 color1, ImVec4 color2, ImVec4 color3, float v_speed = 1.f, float v_min = 0.f, float v_max = 0.f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
	bool ColoredDragFloat(const char* label, float* v, ImVec4 color, float v_speed = 1.f, float v_min = 0.f, float v_max = 0.f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
}