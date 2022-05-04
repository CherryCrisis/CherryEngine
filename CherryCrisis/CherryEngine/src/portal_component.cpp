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
	GetHost().m_cell->RemoveRenderer(&m_portal);
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
	GetHost().m_cell->AddRenderer(&m_portal);
}

void PortalComponent::Initialize()
{
	m_transform = GetHost().GetOrAddBehaviour<Transform>();

	m_transform->m_onPositionChange.Bind(&PortalComponent::ChangePosition, this);
	m_transform->m_onRotationChange.Bind(&PortalComponent::ChangeRotation, this);
	m_transform->m_onScaleChange.Bind(&PortalComponent::ChangeScale, this);

	GetHost().m_OnAwake.Unbind(&PortalComponent::Initialize, this);

	ChangePosition(m_transform->GetPosition());
	ChangeRotation(m_transform->GetRotation());
	ChangeScale(m_transform->GetScale());
}

void PortalComponent::ChangePosition(const CCMaths::Vector3& position)
{
	CCMaths::Vector3 pos = (CCMaths::Vector3)m_transform->position;
	CCMaths::Vector3 rot = (CCMaths::Vector3)m_transform->rotation;

	m_portal.m_viewMatrix = Matrix4::RotateXYZ(-rot) * Matrix4::Translate(-pos);
	m_portal.m_modelMatrix = m_transform->GetWorldMatrix();
}

void PortalComponent::ChangeRotation(const CCMaths::Vector3& rotation)
{
	CCMaths::Vector3 pos = (CCMaths::Vector3)m_transform->position;
	CCMaths::Vector3 rot = (CCMaths::Vector3)m_transform->rotation;

	m_portal.m_viewMatrix = Matrix4::RotateXYZ(-rot) * Matrix4::Translate(-pos);
	m_portal.m_modelMatrix = m_transform->GetWorldMatrix();
}

void PortalComponent::ChangeScale(const CCMaths::Vector3& scale)
{
	m_portal.m_modelMatrix = m_transform->GetWorldMatrix();
}
