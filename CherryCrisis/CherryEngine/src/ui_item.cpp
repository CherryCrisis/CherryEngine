#include <pch.hpp>

#include "ui_item.hpp"

void UIItem::PopulateMetadatas() 
{
	m_metadatas.SetProperty("Name", &Name);
	m_metadatas.SetProperty("Position", &Position);
	m_metadatas.SetProperty("Size", &Size);
}

void UIItem::Delete() {};