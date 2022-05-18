#include <pch.hpp>

#include "ui_item.hpp"
#include "picking_renderpass.hpp"

int UIItem::count = 1;

void UIItem::PopulateMetadatas() 
{
	m_metadatas.SetProperty("Name", &Name);
	m_metadatas.SetProperty("Position", &Position);
	m_metadatas.SetProperty("Size", &Size);
}

void UIItem::Delete() {};

UIItem::UIItem() 
{
	m_id = count;
	count++;
}

UIItem::UIItem(CCUUID& id) : Object(id)
{
	m_id = count;
	count++;
}

void UIItem::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->SubscribeToPipeline<PickingRenderPass>(this);
}

void UIItem::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->UnsubscribeToPipeline<PickingRenderPass>(this);
}