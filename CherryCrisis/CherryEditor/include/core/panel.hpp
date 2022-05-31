#pragma once

#include <string>

#include <imgui.h>


class Panel
{
protected:
	bool m_isOpened = true;
	bool m_isUnique = true;

public:
	Panel(bool spawnOpened = true) : m_isOpened(spawnOpened) {}
	
	virtual void Render() = 0;
	virtual void ContextCallback() {};

	void Toggle(bool state) { m_isOpened = state; }
};

class PanelCategory
{
private:
	std::string m_name = "";

public:
	PanelCategory(std::string name = "default") : m_name(name) {}
	virtual ~PanelCategory() = default;
	
	//Override this to implements the imgui stuff
	virtual void Fill() = 0;

	void DisplayCategory();
	bool DisplayLabel();
};