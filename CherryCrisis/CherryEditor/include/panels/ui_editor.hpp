#pragma once

#include "core/frame_displayer.hpp"

#include <vector>
#include <string>

#include "ui_context.hpp"

class UIItem;
class EditorManager;

class UIEditor : public FrameDisplayer
{
private:
	void SetDraggedItem(const std::string& item, EItemUI type);
	
	void InspectSelectedItem();

	UIItem* m_selectedItem = nullptr;
public:
	EditorManager* m_manager = nullptr;
	UIEditor();
	void Render() override;

	std::vector<std::string> m_items;

};