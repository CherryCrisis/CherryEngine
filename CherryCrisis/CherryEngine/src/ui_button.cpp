#include <pch.hpp>

#include "ui_button.hpp"

#include "resource_manager.hpp"
#include "ui_renderpass.hpp"
#include "ui_text_renderpass.hpp"
#include "picking_renderpass.hpp"
#include "camera_component.hpp"

UIButton::UIButton()
{
	PopulateMetadatas();

	SetName("Button");

	CameraComponent* cam = CameraComponent::GetMainCamera();
	if (!cam)
		return;

	SubscribeToPipeline(cam->m_camera.m_pipeline.get());
}

void UIButton::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->SubscribeToPipeline<PickingRenderPass>((UIItem*)&m_background);
	pipeline->SubscribeToPipeline<UIRenderPass>(&m_background);
	pipeline->SubscribeToPipeline<TextRenderPass>(&m_text);
}

void UIButton::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->UnsubscribeToPipeline<PickingRenderPass>((UIItem*)&m_background);
	pipeline->UnsubscribeToPipeline<UIRenderPass>(&m_background);
	pipeline->UnsubscribeToPipeline<TextRenderPass>(&m_text);
}


void UIButton::PopulateMetadatas()
{
	UIItem::PopulateMetadatas();

	// TODO: Clean this mess

	//----------------------------
	// Text Metadatas
	//----------------------------
	m_metadatas.m_properties["Text"] = m_text.m_metadatas.m_properties["Text"];
	m_metadatas.m_properties["Text Size"] = m_text.m_metadatas.m_properties["Size"];
	m_metadatas.m_properties["Font"] = m_text.m_metadatas.m_properties["Font"];
	m_metadatas.m_properties["Text Position"] = m_text.m_metadatas.m_properties["Position"];

	//----------------------------
	// Image Metadatas
	//----------------------------
	m_metadatas.m_properties["Background"] = m_background.m_metadatas.m_properties["Texture"];
	m_metadatas.m_properties["Background Size"] = m_background.m_metadatas.m_properties["Size"];
	m_metadatas.m_properties["Background Position"] = m_background.m_metadatas.m_properties["Position"];
}

void UIButton::OnSetPos() 
{
	m_text.SetPos(GetPos());

	m_background.SetPos(GetPos());
}

void UIButton::OnSetSize()
{
	m_background.SetSize(GetSize());
}

bool UIButton::CompareId(int id) 
{
	return id == m_id || id == m_text.m_id || id == m_background.m_id;
}

void UIButton::Interact() 
{
	m_onClicked.Invoke();
}