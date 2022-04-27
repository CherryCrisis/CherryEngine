#include "pch.hpp"

#include "portal_component.hpp"

#include "cell.hpp"

#include "transform.hpp"

#include "basic_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "render_manager.hpp"

#include "basic_rendering_pipeline.hpp"

PortalComponent::PortalComponent()
{
	m_portal.m_pipeline = std::make_unique<BasicRPipeline>();
	PopulateMetadatas();
}

PortalComponent::PortalComponent(CCUUID& id) : Behaviour(id)
{
	m_portal.m_pipeline = std::make_unique<BasicRPipeline>();
	PopulateMetadatas();
}

PortalComponent::~PortalComponent()
{
	GetHost().m_cell->RemoveViewer(&m_portal);
}

void PortalComponent::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	// Add property
	m_metadatas.SetField<Portal*>("linked portal", m_portal.m_linkedPortal);
}

void PortalComponent::BindToSignals()
{
	GetHost().m_OnAwake.Bind(&PortalComponent::Initialize, this);

	GetHost().m_cell->AddViewer(&m_portal);
}

void PortalComponent::Initialize()
{
	m_transform = GetHost().GetOrAddBehaviour<Transform>();

	m_transform->m_onPositionChange.Bind(&PortalComponent::ChangePosition, this);
	m_transform->m_onRotationChange.Bind(&PortalComponent::ChangeRotation, this);

	GetHost().m_OnAwake.Unbind(&PortalComponent::Initialize, this);

	ChangePosition(m_transform->GetPosition());
	ChangeRotation(m_transform->GetRotation());
}

void PortalComponent::ChangePosition(const CCMaths::Vector3& position)
{
	m_portal.m_position = position;

	UpdateCameraModel();
}

void PortalComponent::ChangeRotation(const CCMaths::Vector3& rotation)
{
	m_portal.m_rotation = rotation;

	UpdateCameraModel();
}

void PortalComponent::UpdateCameraModel()
{
	m_portal.m_viewMatrix = Matrix4::RotateZXY(-m_portal.m_rotation) * Matrix4::Translate(-m_portal.m_position);
}