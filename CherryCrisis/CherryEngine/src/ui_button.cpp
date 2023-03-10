#include <pch.hpp>

#include "ui_button.hpp"

#include "resource_manager.hpp"

#include "camera_component.hpp"
#include "picking_renderpass.hpp"
#include "ui_renderpass.hpp"
#include "ui_text_renderpass.hpp"
#include "utils.hpp"


UIButton::UIButton()
{
	Init();
}

UIButton::UIButton(CCUUID& id) : UIItem(id)
{
	Init();
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

bool UIButton::IsHovered() 
{
	return m_text.IsHovered() || m_background.IsHovered();
}

void UIButton::SetHovered(bool value)
{
	m_text.SetHovered(value);
	m_background.SetHovered(value);
}

void UIButton::PopulateMetadatas()
{
	UIItem::PopulateMetadatas();

	//----------------------------
	// Text Metadatas
	//----------------------------
	m_metadatas.m_metadatas["Text"] = m_text.m_metadatas.m_metadatas["Text"];
	m_metadatas.m_metadatas["Text Size"] = m_text.m_metadatas.m_metadatas["Size"];
	m_metadatas.m_metadatas["Font"] = m_text.m_metadatas.m_metadatas["Font"];
	m_metadatas.m_metadatas["Text Position"] = m_text.m_metadatas.m_metadatas["Position"];

	//----------------------------
	// Image Metadatas
	//----------------------------
	m_metadatas.m_metadatas["Background"] = m_background.m_metadatas.m_metadatas["Texture"];
	m_metadatas.m_metadatas["Background Size"] = m_background.m_metadatas.m_metadatas["Size"];
	m_metadatas.m_metadatas["Background Position"] = m_background.m_metadatas.m_metadatas["Position"];

	//----------------------------
	// Button Metadatas
	//----------------------------
	m_metadatas.SetProperty("Behaviour", &Behaviour);
}

void UIButton::OnSetPos() 
{
	m_text.SetPos(GetPos());

	m_background.SetPos(GetPos());
}

void UIButton::Init() 
{
	PopulateMetadatas();

	SetName("Button");
	m_isHoverable = true;
	m_text.SetHoverable(true);
	m_background.SetHoverable(true);

	CameraComponent* cam = CameraComponent::GetMainCamera();
	if (!cam)
		return;

	SubscribeToPipeline(cam->m_camera.m_pipeline.get());

	m_background.SetSize(GetSize());
}

void UIButton::OnSetSize()
{
	m_background.SetSize(GetSize());
}

bool UIButton::CompareId(int id) 
{
	return id == m_text.m_id || id == m_background.m_id;
}

void UIButton::Interact() 
{
	m_onClicked.Invoke();
}

void UIButton::SetBehaviourName(const std::string& path)
{
	std::filesystem::path file = path;
	std::string name = file.filename().string();
	behaviour.SetScriptClass(String::ExtractKey(name, '.').c_str());
	behaviourPath = path;
	m_onClicked.Bind(&ButtonBehaviour::OnClick, &behaviour);
}

void UIButton::Delete()
{
	CameraComponent* cam = CameraComponent::GetMainCamera();
	if (!cam)
		return;

	UnsubscribeToPipeline(cam->m_camera.m_pipeline.get());
}