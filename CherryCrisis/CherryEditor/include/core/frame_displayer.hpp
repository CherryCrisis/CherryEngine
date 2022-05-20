#pragma once

#include "core/panel.hpp"

class FrameDisplayer : public Panel 
{
protected:
	bool m_isActive = false;

	void Render() override {};
public:
	bool m_isFocused = false;
	
	virtual void Focus() = 0;
	virtual void Unfocus() = 0;
};
