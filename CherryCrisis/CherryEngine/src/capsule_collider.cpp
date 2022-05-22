#include "pch.hpp"

#include "capsule_collider.hpp"

#include <PxPhysicsAPI.h>

#include "resource_manager.hpp"
#include "physic_manager.hpp"

#include "collider_renderpass.hpp"
#include "camera_component.hpp"
#include "transform.hpp"
#include "entity.hpp"

CapsuleCollider::CapsuleCollider()
{
	PopulateMetadatas();

	m_type = EColliderType::CAPSULE;
}

CapsuleCollider::CapsuleCollider(CCUUID& id) : Collider(id)
{
	PopulateMetadatas();

	m_type = EColliderType::CAPSULE;
}

CapsuleCollider::~CapsuleCollider()
{
	Unregister();

	if (m_transform)
	{
		m_transform->m_onScaleChange.Unbind(&CapsuleCollider::SetEntityScale, this);
		m_transform->m_onPositionChange.Unbind(&CapsuleCollider::SetEntityScale, this);
		m_transform->m_onRotationChange.Unbind(&CapsuleCollider::SetEntityScale, this);
		m_transform->m_OnDestroy.Unbind(&CapsuleCollider::InvalidateTransform, this);
	}

	GetHost().m_cell->RemoveRenderer(this);

	GetHost().m_OnCellAdded.Unbind(&CapsuleCollider::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Unbind(&CapsuleCollider::OnCellRemoved, this);
}

void CapsuleCollider::BindToSignals()
{
	PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

	physicManager->Register(this);
	m_isRegistered = true;

	m_transform = m_physicActor->m_owner->GetOrAddBehaviour<Transform>();

	GetHost().m_OnAwake.Bind(&CapsuleCollider::Initialize, this);

	GetHost().m_cell->AddRenderer(this);

	GetHost().m_OnCellAdded.Bind(&CapsuleCollider::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Bind(&CapsuleCollider::OnCellRemoved, this);
}

void CapsuleCollider::OnCellAdded(Cell* newCell)
{
	newCell->AddRenderer(this);
}

void CapsuleCollider::OnCellRemoved(Cell* newCell)
{
	newCell->RemoveRenderer(this);
}

void CapsuleCollider::Initialize()
{
	m_transform = GetHost().GetOrAddBehaviour<Transform>();

	if (m_transform)
	{
		m_transform->m_onScaleChange.Bind(&CapsuleCollider::SetEntityScale, this);
		m_transform->m_onPositionChange.Bind(&CapsuleCollider::SetEntityScale, this);
		m_transform->m_onRotationChange.Bind(&CapsuleCollider::SetEntityScale, this);
		m_transform->m_OnDestroy.Bind(&CapsuleCollider::InvalidateTransform, this);
	}

	GetHost().m_OnAwake.Unbind(&CapsuleCollider::Initialize, this);

	SetEntityScale(m_transform->GetGlobalScale());
}

void CapsuleCollider::InvalidateTransform()
{
	m_transform = nullptr;
}

void CapsuleCollider::Unregister()
{
	if (m_isRegistered)
	{
		PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

		physicManager->Unregister(this);
		m_isRegistered = false;
	}
}

void CapsuleCollider::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetProperty("Enabled", &isEnabled);
	m_metadatas.SetProperty("Is Trigger", &isTrigger);
	m_metadatas.SetProperty("Local Position", &localPosition);
	m_metadatas.SetProperty("Half height", &editableScale);
	m_metadatas.SetProperty("Radius", &radius);
	m_metadatas.SetProperty("Contact Offset", &contactOffset);
}

void CapsuleCollider::SetEntityScale(const CCMaths::Vector3& s)
{
	CCMaths::Vector3 scale = m_transform->GetGlobalScale();

	m_entityRadius = CCMaths::Max(scale.x, scale.z);
	m_entityScale = scale.y;

	m_totalRadius = m_editableRadius * m_entityRadius;

	m_totalScale = CCMaths::Max(0.0001f, (m_editableScale * m_entityScale) - m_totalRadius);

	ComputeModelMatrices();
}

void CapsuleCollider::SetPxShape()
{
	m_pxShape = m_physicActor->CreateShape(physx::PxCapsuleGeometry(m_totalRadius, m_totalScale));

	physx::PxTransform transform = m_pxShape->getLocalPose();
	physx::PxTransform relativeRot = physx::PxTransform(physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1)));
	m_pxShape->setLocalPose(transform * relativeRot);
	SetPxLocalPos();

	m_pxShape->userData = this;

	SetPxData();
}

void CapsuleCollider::ClearPxShape()
{
	if (m_pxShape)
	{
		m_physicActor->RemoveShape(m_pxShape);
		m_pxShape = nullptr;
	}
}

void CapsuleCollider::ResetPxShape()
{
	if (!m_pxShape)
		return;

	ClearPxShape();
	SetPxShape();
}

void CapsuleCollider::SetPxData()
{
	if (m_pxShape)
	{
		m_pxShape->setContactOffset(m_contactOffset);

		if (!m_isEnabled)
		{
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSIMULATION_SHAPE, false);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eTRIGGER_SHAPE, false);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, false);
		}
		else if (m_isTrigger)
		{
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSIMULATION_SHAPE, false);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eTRIGGER_SHAPE, true);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, true);
		}
		else
		{
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eTRIGGER_SHAPE, false);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSIMULATION_SHAPE, true);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, true);
		}
	}
}

void CapsuleCollider::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	Camera* cam = CameraComponent::m_editorCamera;

	if (!cam || cam->m_pipeline.get() != pipeline)
		return;

	pipeline->SubscribeToPipeline<ColliderRenderPass>(static_cast<Collider*>(this));
}

void CapsuleCollider::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	Camera* cam = CameraComponent::m_editorCamera;

	if (!cam || cam->m_pipeline.get() != pipeline)
		return;

	pipeline->UnsubscribeToPipeline<ColliderRenderPass>(static_cast<Collider*>(this));
}

void CapsuleCollider::SetScale(const float& scale)
{
	m_editableScale = scale;

	m_totalScale = CCMaths::Max(0.0001f, (m_editableScale * m_entityScale) - m_totalRadius);
	ComputeModelMatrices();

	ResetPxShape();
}

void CapsuleCollider::SetRadius(const float& radius)
{
	m_editableRadius = radius;

	m_totalRadius = m_editableRadius * m_entityRadius;
	ComputeModelMatrices();

	ResetPxShape();
}

void CapsuleCollider::ComputeModelMatrices()
{
	if (!m_transform)
		return;

	m_topModel = m_transform->GetWorldMatrix().NormalizedScale() * 
				 CCMaths::Matrix4::Translate(m_localPosition + CCMaths::Vector3::YAxis * m_totalScale) *
				 CCMaths::Matrix4::Scale(m_totalRadius);

	m_botModel = m_transform->GetWorldMatrix().NormalizedScale() *
				 CCMaths::Matrix4::Translate(m_localPosition - CCMaths::Vector3::YAxis * m_totalScale) * 
				 CCMaths::Matrix4::Scale(m_totalRadius);

	m_model = m_transform->GetWorldMatrix().NormalizedScale() * 
				  CCMaths::Matrix4::Translate(m_localPosition) * 
				  CCMaths::Matrix4::Scale({ m_totalRadius, m_totalScale, m_totalRadius });
}

CCMaths::Matrix4 CapsuleCollider::GetModelMatrix()
{
	return m_model;
}

CCMaths::Matrix4 CapsuleCollider::GetTopMatrix()
{
	return m_topModel;
}

CCMaths::Matrix4 CapsuleCollider::GetBotMatrix()
{
	return m_botModel;
}
