#pragma once

class ARenderer
{
public:
	bool m_isVisible = true;

	virtual void Draw() { }

	virtual ~ARenderer() = default;
};