#include <pch.hpp>

#include "ui_context.hpp"

#include "ui_image.hpp"

#include "camera_component.hpp"
void UIContext::AddImage() 
{
	UIImage* img = new UIImage("Internal/Icons/file_icon.png");
	m_items.push_back(img);
	CameraComponent* cam = CameraComponent::GetMainCamera();
	img->SubscribeToPipeline(cam->m_camera.m_pipeline.get());
}

void UIContext::RemoveItem()
{
	// TODO: move this into a special delete function
	CameraComponent* cam = CameraComponent::GetMainCamera();
	UIImage* img = Get<UIImage>("Image");
	if (!img) return;
	img->UnsubscribeToPipeline(cam->m_camera.m_pipeline.get());

	m_items.erase(std::remove(m_items.begin(), m_items.end(), img), m_items.end());
	delete img;
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
	static_assert(std::is_base_of_v<UIItem, ItemT>, "ItemTis not inherited of Behaviour");

	for (UIItem* item : m_items) 
		if (item->GetName() == name) 
		{
			auto castedItem = dynamic_cast<ItemT*>(item);
			if (castedItem) return castedItem;
		}

	return nullptr;
}

template <class ItemT>
ItemT* UIContext::Get(const CCUUID& id)
{
	static_assert(!std::is_same_v<UIItem, ItemT>, "ItemT cannot be a pure Behaviour");
	static_assert(std::is_base_of_v<UIItem, ItemT>, "ItemTis not inherited of Behaviour");

	for (UIItem* item : m_items)
		if (item->GetUUID() == id)
		{
			auto castedItem = dynamic_cast<ItemT*>(item);
			if (castedItem) return castedItem;
		}

	return nullptr;
}