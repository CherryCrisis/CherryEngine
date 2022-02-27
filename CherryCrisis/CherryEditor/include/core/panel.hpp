#pragma once

#include <string>


#include <imgui.h>

class Panel
{
protected:

	bool m_isOpened = true;
	bool m_isUnique = true;

public:

	virtual void Render() = 0;

	void Toggle(bool state) { m_isOpened = state; }

	Panel(bool spawnOpened = true) : m_isOpened(spawnOpened) {}
};

class PanelCategory
{
private:

	std::string m_name = "";

protected:
	//Override this to implements the imgui stuff

public:

	PanelCategory(std::string name = "default") : m_name(name) {}

	virtual void Fill() = 0;

	void DisplayCategory();
	bool DisplayLabel();
};