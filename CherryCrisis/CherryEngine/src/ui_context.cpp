#include <pch.hpp>

#include "ui_context.hpp"

#include "ui_button.hpp"
#include "ui_image.hpp"
#include "ui_text.hpp"
#include "ui_toggle.hpp"
#include "ui_slider.hpp"
#include "ui_progressbar.hpp"
#include "ui_input_text.hpp"
#include "ui_horizontal_layout.hpp"
#include "ui_vertical_layout.hpp"

#include "camera_component.hpp"
void UIContext::AddImage() 
{
	UIImage* img = new UIImage("Internal/Icons/file_icon.png");
	m_items.push_back(img);
	CameraComponent* cam = CameraComponent::GetMainCamera();
	img->SubscribeToPipeline(cam->m_camera.m_pipeline.get());
}

void UIContext::RemoveItem(UIItem* item)
{
	item->Delete();
	m_items.erase(std::remove(m_items.begin(), m_items.end(), item), m_items.end());
	delete item;
}

EItemUI UIContext::GetItemTypeByString(const std::string& str) 
{
	return EItemUI::BUTTON;
}

UIContext::~UIContext() 
{
	while (m_items.size() > 0)
	{
		delete m_items[0];
		m_items.erase(m_items.begin());
	}
}

template <class ItemT>
ItemT* UIContext::Get(const std::string& name)
{
	static_assert(!std::is_same_v<UIItem, ItemT>, "ItemT cannot be a pure Behaviour");
	static_assert(std::is_base_of_v<UIItem, ItemT>, "ItemT is not inherited of Behaviour");

	for (UIItem* item : m_items) 
	{
		if (item->GetName() == name) 
		{
			auto castedItem = dynamic_cast<ItemT*>(item);
			if (castedItem) return castedItem;
		}
	}

	return nullptr;
}

template <class ItemT>
ItemT* UIContext::Get(const CCUUID& id)
{
	static_assert(!std::is_same_v<UIItem, ItemT>, "ItemT cannot be a pure Behaviour");
	static_assert(std::is_base_of_v<UIItem, ItemT>, "ItemT is not inherited of Behaviour");

	for (UIItem* item : m_items) 
	{
		if (item->GetUUID() == id)
		{
			auto castedItem = dynamic_cast<ItemT*>(item);
			if (castedItem) return castedItem;
		}
	}

	return nullptr;
}

template <class ItemT>
ItemT* UIContext::AddItem()
{
	static_assert(!std::is_same_v<UIItem, ItemT>, "ItemT cannot be a pure Behaviour");
	static_assert(std::is_base_of_v<UIItem, ItemT>, "ItemT is not inherited of Behaviour");

	ItemT* rawPtr = new ItemT();

	m_items.push_back(rawPtr);
	return rawPtr;
}

void UIContext::AddItemByType(const EItemUI& type)
{
	switch (type)
	{
	case EItemUI::BUTTON:			 {  AddItem<UIButton>(); break; }
	case EItemUI::IMAGE:			 {  AddItem<UIImage>();  break; }
	case EItemUI::TOGGLE:			 {  AddItem<UIToggle>(); break; }
	case EItemUI::SLIDER:			 {  AddItem<UISlider>(); break; }
	case EItemUI::TEXT:				 {  AddItem<UIText>();   break; }
	case EItemUI::PROGRESS_BAR:		 {  AddItem<UIProgressBar>(); break; }
	case EItemUI::INPUT_TEXT:		 {  AddItem<UIInputText>();   break; }
	case EItemUI::VERTICAL_LAYOUT:	 {  AddItem<UIVerticalLayout>();   break;  }
	case EItemUI::HORIZONTAL_LAYOUT: {  AddItem<UIHorizontalLayout>(); break;   }
	}
}