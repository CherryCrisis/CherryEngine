#pragma once

#include <vector>
#include <string>

#include "core/frame_displayer.hpp"
#include "ui_context.hpp"

class EditorManager;
class UIItem;


class UIEditor : public FrameDisplayer
{
private:
	UIItem* m_selectedItem = nullptr;

	void SetDraggedItem(const std::string& item, EItemUI type);
	
	void InspectSelectedItem();

public:
	std::vector<std::string> m_items;
	
	EditorManager* m_manager = nullptr;

	UIEditor();

	void Render() override;
	void Focus() override {}
	void Unfocus() override {}
};