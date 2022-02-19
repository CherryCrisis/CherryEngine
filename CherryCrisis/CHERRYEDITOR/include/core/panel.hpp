#pragma once

class Panel
{
public:

	virtual void Render() = 0;

	void Toggle(bool state) { m_isOpened = state; }

protected:

	bool m_isOpened = true;
	bool m_isUnique = true;
};