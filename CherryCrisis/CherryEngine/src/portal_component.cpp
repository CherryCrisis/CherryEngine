#include "pch.hpp"

#include "portal_component.hpp"

#include "cell.hpp"

#include "transform.hpp"

#include "basic_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "render_manager.hpp"

#include "mixed_rendering_pipeline.hpp"

PortalComponent::PortalComponent()
{
	m_portal.m_pipeline = std::make_unique<MixedPipeline>();
	PopulateMetadatas();
}

PortalComponent::PortalComponent(CCUUID& id) : Behaviour(id)
{
	m_portal.m_pipeline = std::make_unique<MixedPipeline>();
	PopulateMetadatas();
}

PortalComponent::~PortalComponent()
{
	InvalidateLinkedPortal();

	GetHost().m_cell->RemoveViewer(&m_portal);
	GetHost().m_cell->RemoveRenderer(&m_portal);
}

void PortalComponent::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	// Add property
	m_metadatas.SetProperty("linkedPortalProp", &m_LinkedPortalProp);
}

void PortalComponent::BindToSignals()
{
	GetHost().m_OnAwake.Bind(&PortalComponent::Initialize, this);

	GetHost().m_cell->AddViewer(&m_portal);
	GetHost().m_cell->AddRenderer(&m_portal);
	GetHost().m_OnCellAdded.Bind(&PortalComponent::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Bind(&PortalComponent::OnCellRemoved, this);
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

void PortalComponent::InvalidateLinkedPortal()
{
	if (!m_linkedPortal)
		return;

	m_OnDestroy.Unbind(&PortalComponent::InvalidateLinkedPortal, m_linkedPortal);
	m_linkedPortal->m_OnDestroy.Unbind(&PortalComponent::InvalidateLinkedPortal, this);

	m_portal.m_linkedPortal = m_linkedPortal->m_portal.m_linkedPortal = nullptr;
	m_linkedPortal = m_linkedPortal->m_linkedPortal = nullptr;
};

void PortalComponent::SetLinkedPortal(Object* linkedObject)
{
	InvalidateLinkedPortal();

	auto linkedEntity = dynamic_cast<Entity*>(linkedObject);

	if (!linkedEntity)
		return;

	if (PortalComponent* linkedPortalComp = linkedEntity->GetBehaviour<PortalComponent>())
	{
		m_linkedPortal = linkedPortalComp;

		m_linkedPortal->m_OnDestroy.Bind(&PortalComponent::InvalidateLinkedPortal, this);

		linkedPortalComp->m_linkedPortal = this;
		Portal* tempPortal = &m_portal;
		m_portal.m_linkedPortal = &linkedPortalComp->m_portal;
		linkedPortalComp->m_portal.m_linkedPortal = tempPortal;

		m_linkedPortal->m_OnDestroy.Unbind(&PortalComponent::InvalidateLinkedPortal, this);
	}
}

Object* PortalComponent::GetLinkedPortal()
{
	return m_linkedPortal ? m_linkedPortal->GetHostPtr() : nullptr;
}

void PortalComponent::OnCellAdded(Cell* newCell)
{
	newCell->AddRenderer(&m_portal);
	newCell->AddViewer(&m_portal);
}

void PortalComponent::OnCellRemoved(Cell* newCell)
{
	newCell->RemoveRenderer(&m_portal);
	newCell->RemoveViewer(&m_portal);
}