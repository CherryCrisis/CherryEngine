#include <pch.hpp>

#include "portal_component.hpp"

#include "render_manager.hpp"

#include "cell.hpp"
#include "entity.hpp"
#include "portal_teleporter_component.hpp"
#include "transform.hpp"
#include "mixed_rendering_pipeline.hpp"
#include "transform.hpp"
#include "basic_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "mesh_renderer.hpp"


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

	GetHost().m_OnTick.Unbind(&PortalComponent::LateUpdate, this);

	GetHost().m_cell->RemoveViewer(&m_portal);
	GetHost().m_cell->RemoveRenderer(&m_portal);

	GetHost().m_OnCellAdded.Unbind(&PortalComponent::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Unbind(&PortalComponent::OnCellRemoved, this);

	GetHost().m_OnTriggerEnter.Unbind(&PortalComponent::OnTriggerEnter, this);
	GetHost().m_OnTriggerExit.Unbind(&PortalComponent::OnTriggerExit, this);
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
	GetHost().m_OnTick.Bind(&PortalComponent::LateUpdate, this);
	GetHost().m_OnTriggerEnter.Bind(&PortalComponent::OnTriggerEnter, this);
	GetHost().m_OnTriggerExit.Bind(&PortalComponent::OnTriggerExit, this);

	GetHost().m_cell->AddViewer(&m_portal);
	GetHost().m_cell->AddRenderer(&m_portal);
	GetHost().m_OnCellAdded.Bind(&PortalComponent::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Bind(&PortalComponent::OnCellRemoved, this);
}

void PortalComponent::Initialize()
{
	m_transform = GetHost().GetOrAddBehaviour<Transform>();

	m_transform->m_onPositionChange.Bind(&PortalComponent::UpdatePortalMatrices, this);
	m_transform->m_onRotationChange.Bind(&PortalComponent::UpdatePortalMatrices, this);
	m_transform->m_onScaleChange.Bind(&PortalComponent::UpdatePortalMatrices, this);

	GetHost().m_OnAwake.Unbind(&PortalComponent::Initialize, this);
	
	UpdatePortalMatrices(m_transform);
}

void PortalComponent::LateUpdate()
{
	for (PortalTeleporterComponent* portalTeleporter : m_portalTeleporters)
	{
		Transform* transform = portalTeleporter->m_entityNode->m_transform;

		Vector3 offsetFromPortal = transform->GetPosition() - m_transform->GetPosition();
		int portalSide = CCMaths::Sign<float>(Vector3::Dot(offsetFromPortal, -m_transform->GetWorldMatrix().back.Normalized()));
		int previousPortalSide = CCMaths::Sign<float>(Vector3::Dot(portalTeleporter->m_previousOffsetFromPortal, -m_transform->GetWorldMatrix().back.Normalized()));

		Matrix4 worldMatrixLinkedPortal = m_portal.m_linkedPortal->m_modelMatrix/* * Matrix4::RotateY(CCMaths::PI)*/;
		Matrix4 worldMatrix = worldMatrixLinkedPortal *
			m_portal.m_modelMatrix.Inverse() * 
			transform->GetWorldMatrix();

		CCMaths::Vector3 TRS[3] = {};
		Matrix4::Decompose(worldMatrix, TRS[0], TRS[1], TRS[2]);

		//Teleport entity if it has crossed from one side of portal to the other
		if (portalSide != previousPortalSide)
		{
			portalTeleporter->Teleport(m_linkedPortal, TRS[0], TRS[1], TRS[2]);

			portalTeleporter->ExitPortal();
			m_linkedPortal->OnEntityEnter(portalTeleporter);
			m_portalTeleporters.erase(portalTeleporter);
			return;
		}
		else
		{
			portalTeleporter->m_previousOffsetFromPortal = offsetFromPortal;
		}

		portalTeleporter->UpdateEntityMatrix(portalTeleporter->m_cloneEntityNode->m_transform, TRS[0], TRS[1], TRS[2]);
		UpdateSliceParamaters(portalTeleporter);

		//TODO: portal clipping with portal and linkedPortal 
		/*
		float ProtectScreenFromClipping (Vector3 viewPoint)
		{
		float halfHeight = playerCam.nearClipPlane * Mathf.Tan (playerCam.fieldOfView * 0.5f * Mathf.Deg2Rad);
        float halfWidth = halfHeight * playerCam.aspect;
        float dstToNearClipPlaneCorner = new Vector3 (halfWidth, halfHeight, playerCam.nearClipPlane).magnitude;
        float screenThickness = dstToNearClipPlaneCorner;

        Transform screenT = screen.transform;
        bool camFacingSameDirAsPortal = Vector3.Dot (transform.forward, transform.position - viewPoint) > 0;
        screenT.localScale = new Vector3 (screenT.localScale.x, screenT.localScale.y, screenThickness);
        screenT.localPosition = Vector3.forward * screenThickness * ((camFacingSameDirAsPortal) ? 0.5f : -0.5f);
		}*/
	}
}

void PortalComponent::UpdateSliceParamaters(PortalTeleporterComponent* portalTeleporter)
{
	Transform* transform = portalTeleporter->m_entityNode->m_transform;

	Vector3 offsetFromPortal = transform->GetPosition() - m_transform->GetPosition();
	int portalSide = CCMaths::Sign<float>(Vector3::Dot(offsetFromPortal, -m_transform->GetWorldMatrix().back.Normalized()));

	Vector3 sliceNormal = (-m_transform->GetWorldMatrix().back.Normalized()) * (-static_cast<float>(portalSide));
	Vector3 sliceCentre = m_transform->GetPosition();

	Vector3 cloneSliceNormal = (-m_linkedPortal->m_transform->GetWorldMatrix().back.Normalized()) * static_cast<float>(portalSide);
	Vector3 cloneSliceCentre = m_linkedPortal->m_transform->GetPosition();

	portalTeleporter->SetSliceParams(portalTeleporter->m_entityNode.get(), true, sliceCentre, sliceNormal);
	portalTeleporter->SetSliceParams(portalTeleporter->m_cloneEntityNode.get(), true, cloneSliceCentre, cloneSliceNormal);
}


void PortalComponent::UpdatePortalMatrices(Transform* tranform)
{
	m_portal.m_modelMatrix = m_transform->GetWorldMatrix();

	//if (m_portal.m_linkedPortal)
	//	m_portal.m_viewLinkedPortal = m_portal.m_linkedPortal->m_modelMatrix * m_portal.m_modelMatrix.Inverse();
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

void PortalComponent::SetLinkedPortal(Behaviour* linkedObject)
{
	InvalidateLinkedPortal();

	if (!linkedObject)
		return;

	PortalComponent* linkedPortal = static_cast<PortalComponent*>(linkedObject);

	m_linkedPortal = linkedPortal;

	m_linkedPortal->m_OnDestroy.Bind(&PortalComponent::InvalidateLinkedPortal, this);

	linkedPortal->m_linkedPortal = this;
	Portal* tempPortal = &m_portal;
	m_portal.m_linkedPortal = &linkedPortal->m_portal;
	linkedPortal->m_portal.m_linkedPortal = tempPortal;

	m_linkedPortal->m_OnDestroy.Unbind(&PortalComponent::InvalidateLinkedPortal, this);
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

	Matrix4 worldMatrixLinkedPortal = m_portal.m_linkedPortal->m_modelMatrix/* * Matrix4::RotateY(CCMaths::PI)*/;
	Matrix4 worldMatrix = worldMatrixLinkedPortal *
		m_portal.m_modelMatrix.Inverse() *
		portalTeleporter->m_entityNode->m_transform->GetWorldMatrix();

	CCMaths::Vector3 TRS[3] = {};
	Matrix4::Decompose(worldMatrix, TRS[0], TRS[1], TRS[2]);

	portalTeleporter->EnterPortal(m_linkedPortal, TRS[0], TRS[1], TRS[2]);
	portalTeleporter->m_previousOffsetFromPortal = portalTeleporter->m_entityNode->m_transform->GetPosition() - m_transform->GetPosition();
	m_portalTeleporters.insert(portalTeleporter);

	UpdateSliceParamaters(portalTeleporter);
}

void PortalComponent::OnTriggerEnter(Entity* other)
{
	if (PortalTeleporterComponent* portalTeleporter = other->GetBehaviour<PortalTeleporterComponent>())
		OnEntityEnter(portalTeleporter);
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
