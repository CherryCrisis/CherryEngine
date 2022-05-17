#include <pch.hpp>

#include "ui_text.hpp"

#include "resource_manager.hpp"
#include "ui_text_renderpass.hpp"
#include "camera_component.hpp"

UIText::UIText() 
{
	//Load font
	SetFont("Assets/Fonts/Urzeit.ttf");

	PopulateMetadatas();

	SetName("Text");

	CameraComponent* cam = CameraComponent::GetMainCamera();
	if (!cam)
		return;

	SubscribeToPipeline(cam->m_camera.m_pipeline.get());
}

void UIText::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	UIItem::SubscribeToPipeline(pipeline);
	pipeline->SubscribeToPipeline<TextRenderPass>(this);
}

void UIText::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	UIItem::UnsubscribeToPipeline(pipeline);
	pipeline->UnsubscribeToPipeline<TextRenderPass>(this);
}

void UIText::SetFont(const std::string& path) 
{
	m_font = ResourceManager::GetInstance()->AddResource<Font>(path.c_str(), true);
	m_fontPath = m_font->m_filepath.string();
}

void UIText::PopulateMetadatas()
{
	UIItem::PopulateMetadatas();

	m_metadatas.SetProperty("Font", &FontPath);
	m_metadatas.SetProperty("Text", &Text);
	//m_metadatas.SetProperty("Font Size", &Size);
}