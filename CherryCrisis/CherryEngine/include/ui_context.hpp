#pragma once

#include <vector>

#include <cherry_macros.hpp>
#include "ui_item.hpp"

enum class EItemUI
{
	IMAGE,
	HORIZONTAL_LAYOUT,
	VERTICAL_LAYOUT,
	TEXT,
	PROGRESS_BAR,
	BUTTON,
	TOGGLE,
	SLIDER,
	INPUT_TEXT
};

//Container that represents the UI.
//One instance per scene
class CCENGINE_API UIContext 
{
public:
	template<class ItemT>
	ItemT* AddItem();
	void AddItemByType(const EItemUI& type);
	void RemoveItem(UIItem* item);
	
	std::vector<UIItem*> m_items;

	template<class ItemT>
	ItemT* Get(const CCUUID& id);
	template<class ItemT>
	ItemT* Get(const std::string& name);

	UIItem* Get(uint32_t id);
	~UIContext();
};