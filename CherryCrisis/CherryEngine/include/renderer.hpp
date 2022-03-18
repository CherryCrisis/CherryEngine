#pragma once

#include "cherry_macros.hpp"

class CCENGINE_API ARenderer
{
public:
	bool m_isVisible = true;

	virtual void Draw() { }

	virtual ~ARenderer() = default;
};