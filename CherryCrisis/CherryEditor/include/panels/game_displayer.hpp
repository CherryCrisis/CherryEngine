#pragma once

#include "core/frame_displayer.hpp"

class GameDisplayer : public FrameDisplayer
{
public:
	bool m_isHovered = false;
	bool m_isFocused = false;

	void Render() override;
};