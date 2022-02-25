#pragma once

class Panel
{
public:

	virtual void Render() = 0;

	void Toggle(bool state) { m_isOpened = state; }

	Panel(bool spawnOpened = true) : m_isOpened(spawnOpened) {}
protected:

	bool m_isOpened = true;
	bool m_isUnique = true;
};