#pragma once

#include "core/frame_displayer.hpp"

#include <vector>
#include <string>

#include "ui_context.hpp"

class UIItem;

class UIEditor : public FrameDisplayer
{
private:
	void SetDraggedItem(const std::string& item, EItemUI type);
	void AddItem(const std::string& item);
	
	void InspectSelectedItem();

	UIItem* m_selectedItem = nullptr;
public:
	UIEditor();
	void Render() override;

	std::vector<std::string> m_items;

};