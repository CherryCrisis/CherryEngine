#pragma once

#include <vector>

#include <cherry_macros.hpp>
#include "ui_item.hpp"

enum class EItemUI
{
	IMAGE,
	BUTTON
};

//Container that represents the UI.
//One instance per scene
class CCENGINE_API UIContext 
{
public:
	void AddImage();
	void RemoveItem();
	
	std::vector<UIItem*> m_items;

	template<class ItemT>
	ItemT* Get(const CCUUID& id);
	template<class ItemT>
	ItemT* Get(const std::string& name);

	static EItemUI GetItemTypeByString(const std::string& str);

	~UIContext();
};