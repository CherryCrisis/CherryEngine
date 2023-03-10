#include <pch.hpp>

#include "box_collider.hpp"

#include <PxPhysicsAPI.h>

#include "resource_manager.hpp"
#include "physic_manager.hpp"

#include "camera_component.hpp"
#include "collider_renderpass.hpp"
#include "entity.hpp"
#include "shape_renderer.hpp"
#include "transform.hpp"


BoxCollider::BoxCollider()
{
	PopulateMetadatas();

	m_type = EColliderType::BOX;

	m_isAddedFromInspector = true;
}

BoxCollider::BoxCollider(CCUUID& id) : Collider(id)
{
	PopulateMetadatas();

	m_type = EColliderType::BOX;
}

BoxCollider::~BoxCollider()
{
	Unregister();

	GetHost().m_OnSelected.Unbind(&BoxCollider::Visible, this);
	GetHost().m_OnUnselected.Unbind(&BoxCollider::Unvisible, this);

	if (m_transform)
	{
		m_transform->m_onScaleChange.Unbind(&BoxCollider::SetEntityScale, this);
		m_transform->m_onPositionChange.Unbind(&BoxCollider::RecomputeMatrix, this);
		m_transform->m_onRotationChange.Unbind(&BoxCollider::RecomputeMatrix, this);
		m_transform->m_OnDestroyed.Unbind(&BoxCollider::InvalidateTransform, this);
	}

	GetHost().m_cell->RemoveRenderer(this);

	GetHost().m_OnCellAdded.Unbind(&BoxCollider::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Unbind(&BoxCollider::OnCellRemoved, this);
}

void BoxCollider::BindToSignals()
{
	PhysicSystem::PhysicManager::Register(this);
	m_isRegistered = true;

	GetHost().m_OnAwake.Bind(&BoxCollider::Initialize, this);
	GetHost().m_OnSelected.Bind(&BoxCollider::Visible, this);
	GetHost().m_OnUnselected.Bind(&BoxCollider::Unvisible, this);

	GetHost().m_cell->AddRenderer(this);

	GetHost().m_OnCellAdded.Bind(&BoxCollider::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Bind(&BoxCollider::OnCellRemoved, this);
}

void BoxCollider::OnCellAdded(Cell* newCell)
{
	newCell->AddRenderer(this);
}

void BoxCollider::OnCellRemoved(Cell* newCell)
{
	newCell->RemoveRenderer(this);
}

void BoxCollider::Initialize()
{
	m_transform = GetHost().GetOrAddBehaviour<Transform>();

	if (m_transform)
	{
		m_transform->m_onScaleChange.Bind(&BoxCollider::SetEntityScale, this);
		m_transform->m_onPositionChange.Bind(&BoxCollider::RecomputeMatrix, this);
		m_transform->m_onRotationChange.Bind(&BoxCollider::RecomputeMatrix, this);
		m_transform->m_OnDestroyed.Bind(&BoxCollider::InvalidateTransform, this);
	}

	GetHost().m_OnAwake.Unbind(&BoxCollider::Initialize, this);

	if (m_isAddedFromInspector)
		SetAABBScale();

	SetEntityScale(m_transform);

	m_physicActor->Init();
}

void BoxCollider::InvalidateTransform()
{
	m_transform = nullptr;
}

void BoxCollider::Unregister(bool checkEmpty)
{
	if (m_isRegistered)
	{
		PhysicSystem::PhysicManager::Unregister(this, checkEmpty);
		m_isRegistered = false;
	}
}

void BoxCollider::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetProperty("Enabled", &isEnabled);
	m_metadatas.SetProperty("Is Trigger", &isTrigger);
	m_metadatas.SetProperty("Local Position", &localPosition);
	m_metadatas.SetProperty("Scale", &editableScale);
	m_metadatas.SetProperty("Block Raycast", &isBlocking);
}

void BoxCollider::SetAABBScale()
{
	MeshRenderer* renderer = nullptr;
	renderer = GetHost().GetBehaviour<ModelRenderer>();

	if (!renderer)
		renderer = GetHost().GetBehaviour<ShapeRenderer>();

	if (renderer)
	{
		if (renderer->m_mesh)
		{
			m_editableScale = renderer->m_mesh->m_aabb.m_extents;

			m_localPosition = renderer->m_mesh->m_aabb.m_center;
		}
	}
}

void BoxCollider::SetEntityScale(Transform* transform)
{
	m_entityScale = transform->GetGlobalScale();

	m_totalScale = m_editableScale;
	m_totalScale *= m_entityScale;

	if (m_totalScale.x < 0.01f)
		m_totalScale.x = 0.01f;
	if (m_totalScale.y < 0.01f)
		m_totalScale.y = 0.01f;
	if (m_totalScale.z < 0.01f)
		m_totalScale.z = 0.01f;

	ComputeModelMatrices();

	ResetPxShape();
}

void BoxCollider::SetPxShape()
{
	physx::PxVec3 scalePx = { m_totalScale.x, m_totalScale.y, m_totalScale.z };
	m_pxShape = m_physicActor->CreateShape(physx::PxBoxGeometry(scalePx));

	SetPxLocalPos();

	m_pxShape->userData = this;
	SetPxData();
}

void BoxCollider::ClearPxShape()
{
	if (m_pxShape)
	{
		m_physicActor->RemoveShape(m_pxShape);
		m_pxShape = nullptr;
	}
}

void BoxCollider::ResetPxShape()
{
	if (!m_pxShape)
		return;

	ClearPxShape();
	SetPxShape();
}

void BoxCollider::SetPxData()
{
	if (m_pxShape)
	{
		m_pxShape->setContactOffset(0.02f);

		if (!m_isEnabled)
		{
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSIMULATION_SHAPE, false);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eTRIGGER_SHAPE, false);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, false);
		}
		else
		{
			if (m_isTrigger)
			{
				m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSIMULATION_SHAPE, false);
				m_pxShape->setFlag(physx::PxShapeFlag::Enum::eTRIGGER_SHAPE, true);
			}
			else
			{
				m_pxShape->setFlag(physx::PxShapeFlag::Enum::eTRIGGER_SHAPE, false);
				m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSIMULATION_SHAPE, true);
			}

			if (m_isBlockingRaycast)
				m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, true);
			else
				m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, false);
		}
	}
}

void BoxCollider::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	Camera* cam = CameraComponent::m_editorCamera;

	if (!cam || cam->m_pipeline.get() != pipeline)
		return;

	pipeline->SubscribeToPipeline<ColliderRenderPass>(static_cast<Collider*>(this));
}

void BoxCollider::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	Camera* cam = CameraComponent::m_editorCamera;

	if (!cam || cam->m_pipeline.get() != pipeline)
		return;

	pipeline->UnsubscribeToPipeline<ColliderRenderPass>(static_cast<Collider*>(this));
}

void BoxCollider::SetScale(const CCMaths::Vector3& scale)
{
	m_editableScale = scale;

	m_totalScale = m_editableScale;
	m_totalScale *= m_entityScale;

	if (m_totalScale.x < 0.01f)
		m_totalScale.x = 0.01f;
	if (m_totalScale.y < 0.01f)
		m_totalScale.y = 0.01f;
	if (m_totalScale.z < 0.01f)
		m_totalScale.z = 0.01f;

	ComputeModelMatrices();

	ResetPxShape();
}

void BoxCollider::ComputeModelMatrices()
{
	if (!m_transform)
		return;

	m_model = m_transform->GetWorldMatrix().NormalizedScale() *
				CCMaths::Matrix4::Translate(m_localPosition) *
				CCMaths::Matrix4::Scale(m_totalScale);
}

CCMaths::Matrix4 BoxCollider::GetModelMatrix()
{
	return m_model;
}

void BoxCollider::Copy(Behaviour* copy)
{
	Collider::Copy(copy);

	BoxCollider* copiedCollider = dynamic_cast<BoxCollider*>(copy);

	m_entityScale = copiedCollider->m_entityScale;
	SetScale(copiedCollider->m_editableScale);

	Initialize();
	ResetPxShape();
}