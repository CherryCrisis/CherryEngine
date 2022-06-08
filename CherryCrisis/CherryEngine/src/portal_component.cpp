#include <pch.hpp>

#include "portal_component.hpp"

#include "render_manager.hpp"
#include "debug.hpp"

#include "cell.hpp"
#include "entity.hpp"
#include "portal_teleporter_component.hpp"
#include "transform.hpp"
#include "mixed_rendering_pipeline.hpp"
#include "transform.hpp"
#include "basic_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "mesh_renderer.hpp"
#include "box_collider.hpp"
#include "guizmo_renderpass.hpp"
#include "camera_component.hpp"


PortalComponent::PortalComponent()
{
	m_portal.m_pipeline = std::make_unique<MixedPipeline>();
	PopulateMetadatas();
	SubscribeGuizmo();
}

PortalComponent::PortalComponent(CCUUID& id) : Behaviour(id)
{
	m_portal.m_pipeline = std::make_unique<MixedPipeline>();
	PopulateMetadatas();
	SubscribeGuizmo();
}

PortalComponent::~PortalComponent()
{
	UnsubscribeGuizmo();
	InvalidateLinkedPortal();

	GetHost().m_OnLateTick.Unbind(&PortalComponent::LateUpdate, this);

	GetHost().m_cell->RemoveViewer(&m_portal);
	GetHost().m_cell->RemoveRenderer(&m_portal);

	GetHost().m_OnCellAdded.Unbind(&PortalComponent::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Unbind(&PortalComponent::OnCellRemoved, this);

	GetHost().m_OnTriggerEnter.Unbind(&PortalComponent::OnTriggerEnter, this);
	GetHost().m_OnTriggerExit.Unbind(&PortalComponent::OnTriggerExit, this);

	if (m_linkedPortal)
		m_linkedPortal->m_OnDestroy.Unbind(&PortalComponent::InvalidateLinkedPortal, this);
}

void PortalComponent::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	// Add property
	m_metadatas.SetProperty("linkedPortalProp", &m_LinkedPortalProp, "PortalComponent");
}

void PortalComponent::BindToSignals()
{
	GetHost().m_OnAwake.Bind(&PortalComponent::Initialize, this);
	GetHost().m_OnLateTick.Bind(&PortalComponent::LateUpdate, this);
	GetHost().m_OnTriggerEnter.Bind(&PortalComponent::OnTriggerEnter, this);
	GetHost().m_OnTriggerExit.Bind(&PortalComponent::OnTriggerExit, this);

	GetHost().m_cell->AddViewer(&m_portal);
	GetHost().m_cell->AddRenderer(&m_portal);
	GetHost().m_OnCellAdded.Bind(&PortalComponent::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Bind(&PortalComponent::OnCellRemoved, this);
}

void PortalComponent::Initialize()
{
	if (!m_transform)
		m_transform = GetHost().GetOrAddBehaviour<Transform>();

	BoxCollider* boxCollider = GetHost().GetBehaviour<BoxCollider>();

	if (!boxCollider)
	{
		boxCollider = GetHost().AddBehaviour<BoxCollider>();
		boxCollider->Initialize();
		boxCollider->SetScale(m_boxColliderScale);
		boxCollider->SetTrigger(true);
	}

	UpdatePortalMatrices();

	if (m_linkedPortal)
	{
		if (!m_linkedPortal->m_transform)
		{
			Transform* linkedTransform = m_linkedPortal->GetHost().GetOrAddBehaviour<Transform>();
			m_linkedPortal->m_transform = linkedTransform;
		}
		
		m_linkedPortal->UpdatePortalMatrices();
	}

	UpdateRelativeLinkedPortalMatrix();


	m_transform->m_onPositionChange.Bind(&PortalComponent::UpdatePortalMatrices, this);
	m_transform->m_onRotationChange.Bind(&PortalComponent::UpdatePortalMatrices, this);
	m_transform->m_onScaleChange.Bind(&PortalComponent::UpdatePortalMatrices, this);

	GetHost().m_OnAwake.Unbind(&PortalComponent::Initialize, this);
	
}

void PortalComponent::LateUpdate()
{
	for (PortalTeleporterComponent* portalTeleporter : m_portalTeleporters)
	{
		Transform* transform = portalTeleporter->m_entityNode->m_transform;

		Vector3 portalForward = -m_transform->GetWorldMatrix().back.Normalized();

		Vector3 offsetFromPortal = transform->GetGlobalPosition() - m_transform->GetGlobalPosition();

		//Get distance from normal (portal)
		float distFromPortal = Vector3::Dot(offsetFromPortal, portalForward);
		float previousDist = Vector3::Dot(portalTeleporter->m_previousOffsetFromPortal, portalForward);

		int portalSideNoOffset = CCMaths::Sign<float>(distFromPortal);

		//Clamp position if portalTeleported exceeds the portal position
		if (CCMaths::Sign(previousDist) != CCMaths::Sign(distFromPortal))
		{
			Vector3 newPosition = transform->GetGlobalPosition() - (portalForward * (distFromPortal + ((m_offset * 0.5f)  * (float)portalSideNoOffset)));

			Vector3 previousPosition = transform->GetGlobalPosition() - (portalForward * (distFromPortal + ((m_offset) * (float)portalSideNoOffset)));

			transform->SetGlobalPosition(newPosition);

			offsetFromPortal = newPosition - m_transform->GetGlobalPosition();
			distFromPortal = Vector3::Dot(offsetFromPortal, portalForward);

			portalTeleporter->m_previousOffsetFromPortal = previousPosition - m_transform->GetGlobalPosition();
			previousDist = Vector3::Dot(portalTeleporter->m_previousOffsetFromPortal, portalForward);

			portalSideNoOffset = CCMaths::Sign<float>(distFromPortal);
		}

		Vector3 newOffsetFromPortal = offsetFromPortal - (portalForward * m_offset) * ((float)portalSideNoOffset);

		int portalSide = CCMaths::Sign<float>(Vector3::Dot(newOffsetFromPortal, portalForward));
		int previousPortalSide = CCMaths::Sign<float>(Vector3::Dot(portalTeleporter->m_previousOffsetFromPortal, portalForward));

		Matrix4 portalTeleporterMatrix = transform->GetWorldMatrix();
		Matrix4 worldMatrixPortals = m_portal.m_linkedPortal->m_modelMatrix * m_portal.m_modelMatrix.Inverse();

		bool portalSideDiff = (portalSide != previousPortalSide);
		bool distCheck = (CCMaths::Abs(previousDist) > CCMaths::Abs(distFromPortal));

		//Teleport entity if it has crossed from one side of portal to the other
		if (portalSideDiff && distCheck)
		{
			portalTeleporterMatrix.position -= portalForward * (distFromPortal - (m_offset * (float)portalSide));

			Matrix4 worldMatrix = worldMatrixPortals * portalTeleporterMatrix;

			CCMaths::Vector3 TRS[3] = {};
			Matrix4::Decompose(worldMatrix, TRS[0], TRS[1], TRS[2]);

			portalTeleporter->Teleport(m_linkedPortal, TRS[0], TRS[1], TRS[2]);

			portalTeleporter->ExitPortal();
			m_portalTeleporters.erase(portalTeleporter);

			m_linkedPortal->OnEntityEnter(portalTeleporter);

			return;
		}
		else
		{
			portalTeleporter->m_previousOffsetFromPortal = offsetFromPortal;
		}

		Matrix4 worldMatrix = worldMatrixPortals * portalTeleporterMatrix;

		portalTeleporter->UpdateEntityMatrix(portalTeleporter->m_cloneEntityNode->m_transform, worldMatrix);
		UpdateSliceParamaters(portalTeleporter);
	}
}


void PortalComponent::UpdateSliceParamaters(PortalTeleporterComponent* portalTeleporter)
{
	Transform* transform = portalTeleporter->m_entityNode->m_transform;

	Vector3 offsetFromPortal = transform->GetGlobalPosition() - m_transform->GetGlobalPosition();
	int portalSide = CCMaths::Sign<float>(Vector3::Dot(offsetFromPortal, -m_transform->GetWorldMatrix().back.Normalized()));

	Vector3 sliceNormal = (-m_transform->GetWorldMatrix().back.Normalized()) * (-static_cast<float>(portalSide));
	Vector3 sliceCentre = m_transform->GetGlobalPosition();

	Vector3 cloneSliceNormal = (-m_linkedPortal->m_transform->GetWorldMatrix().back.Normalized()) * static_cast<float>(portalSide);
	Vector3 cloneSliceCentre = m_linkedPortal->m_transform->GetGlobalPosition();

	portalTeleporter->SetSliceParams(portalTeleporter->m_entityNode.get(), true, sliceCentre, sliceNormal);
	portalTeleporter->SetSliceParams(portalTeleporter->m_cloneEntityNode.get(), true, cloneSliceCentre, cloneSliceNormal);
}


void PortalComponent::UpdatePortalMatrices(Transform* tranform)
{
	UpdateModelMatrix();
	UpdateRelativeLinkedPortalMatrix();
}

void PortalComponent::UpdateModelMatrix()
{
	if (m_transform)
		m_portal.m_modelMatrix = m_transform->GetWorldMatrix();
}

void PortalComponent::UpdateRelativeLinkedPortalMatrix()
{
	if (m_linkedPortal)
		m_portal.m_relativeLinkedPortalMatrix = m_linkedPortal->m_portal.m_modelMatrix * m_portal.m_modelMatrix.Inverse();
}

void PortalComponent::InvalidateLinkedPortal()
{
	if (!m_linkedPortal)
		return;

	m_linkedPortal->m_OnDestroy.Unbind(&PortalComponent::InvalidateLinkedPortal, this);
	m_portal.m_linkedPortal = nullptr;
	m_linkedPortal  = nullptr;
};

void PortalComponent::SetLinkedPortal(Behaviour* linkedObject)
{
	InvalidateLinkedPortal();

	if (!linkedObject)
		return;

	PortalComponent* linkedPortal = static_cast<PortalComponent*>(linkedObject);

	m_linkedPortal = linkedPortal;

	m_linkedPortal->m_OnDestroy.Bind(&PortalComponent::InvalidateLinkedPortal, this);

	m_portal.m_linkedPortal = &linkedPortal->m_portal;

	UpdateModelMatrix();
	m_linkedPortal->UpdateModelMatrix();

	UpdateRelativeLinkedPortalMatrix();
}

Behaviour* PortalComponent::GetLinkedPortal()
{
	return m_linkedPortal;
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

void PortalComponent::OnEntityEnter(PortalTeleporterComponent* portalTeleporter)
{
	if (m_portalTeleporters.end() != m_portalTeleporters.find(portalTeleporter))
		return;

	Matrix4 worldMatrixLinkedPortal = m_portal.m_linkedPortal->m_modelMatrix;
	Matrix4 worldMatrix = worldMatrixLinkedPortal *
		m_portal.m_modelMatrix.Inverse() *
		portalTeleporter->m_entityNode->m_transform->GetWorldMatrix();

	CCMaths::Vector3 TRS[3] = {};
	Matrix4::Decompose(worldMatrix, TRS[0], TRS[1], TRS[2]);

	portalTeleporter->EnterPortal(m_linkedPortal, TRS[0], TRS[1], TRS[2]);
	portalTeleporter->m_previousOffsetFromPortal = portalTeleporter->m_entityNode->m_transform->GetGlobalPosition() - m_transform->GetGlobalPosition();
	m_portalTeleporters.insert(portalTeleporter);

	UpdateSliceParamaters(portalTeleporter);
}

void PortalComponent::OnTriggerEnter(Entity* other)
{
	if (PortalTeleporterComponent* portalTeleporter = other->GetBehaviour<PortalTeleporterComponent>())
	{
		OnEntityEnter(portalTeleporter);
	}
}

void PortalComponent::OnTriggerExit(Entity* other)
{
	if (PortalTeleporterComponent* portalTeleporter = other->GetBehaviour<PortalTeleporterComponent>())
	{
		auto it = m_portalTeleporters.find(portalTeleporter);
		if (it != m_portalTeleporters.end())
		{
			portalTeleporter->ExitPortal();
			m_portalTeleporters.erase(it);
		}
	}
}

void PortalComponent::SubscribeGuizmo()
{
	if (CameraComponent::m_editorCamera)
		CameraComponent::m_editorCamera->m_pipeline->SubscribeToPipeline<GuizmoRenderPass>(this);
}

void PortalComponent::UnsubscribeGuizmo()
{
	if (CameraComponent::m_editorCamera)
		CameraComponent::m_editorCamera->m_pipeline->UnsubscribeToPipeline<GuizmoRenderPass>(this);
}