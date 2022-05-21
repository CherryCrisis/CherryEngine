#include <pch.hpp>

#include "sphere_collider.hpp"

#include <PxPhysicsAPI.h>

#include "physic_manager.hpp"
#include "resource_manager.hpp"

#include "camera_component.hpp"
#include "collider_renderpass.hpp"
#include "entity.hpp"
#include "transform.hpp"


SphereCollider::SphereCollider()
{
	PopulateMetadatas();

	m_type = EColliderType::SPHERE;

	Camera* cam = CameraComponent::m_editorCamera;
	if (!cam)
		return;

	SubscribeToPipeline(cam->m_pipeline.get());
}

SphereCollider::SphereCollider(CCUUID& id) : Collider(id)
{
	PopulateMetadatas();

	m_type = EColliderType::SPHERE;

	Camera* cam = CameraComponent::m_editorCamera;
	if (!cam)
		return;

	SubscribeToPipeline(cam->m_pipeline.get());
}

SphereCollider::~SphereCollider()
{
	Unregister();

	if (Camera* cam = CameraComponent::m_editorCamera)
		UnsubscribeToPipeline(cam->m_pipeline.get());

	GetHost().m_OnSelected.Unbind(&SphereCollider::Visible, this);
	GetHost().m_OnUnselected.Unbind(&SphereCollider::Unvisible, this);

	if (m_transform)
	{
		m_transform->m_onScaleChange.Unbind(&SphereCollider::SetEntityScale, this);
		m_transform->m_onPositionChange.Unbind(&SphereCollider::RecomputeMatrix, this);
		m_transform->m_onRotationChange.Unbind(&SphereCollider::RecomputeMatrix, this);
		m_transform->m_OnDestroy.Unbind(&SphereCollider::InvalidateTransform, this);
	}
}

void SphereCollider::BindToSignals()
{
	PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

	physicManager->Register(this);
	m_isRegistered = true;

	GetHost().m_OnAwake.Bind(&SphereCollider::Initialize, this);
	GetHost().m_OnSelected.Bind(&SphereCollider::Visible, this);
	GetHost().m_OnUnselected.Bind(&SphereCollider::Unvisible, this);
}

void SphereCollider::Initialize()
{
	m_transform = GetHost().GetOrAddBehaviour<Transform>();

	if (m_transform)
	{
		m_transform->m_onScaleChange.Bind(&SphereCollider::SetEntityScale, this);
		m_transform->m_onPositionChange.Bind(&SphereCollider::RecomputeMatrix, this);
		m_transform->m_onRotationChange.Bind(&SphereCollider::RecomputeMatrix, this);
		m_transform->m_OnDestroy.Bind(&SphereCollider::InvalidateTransform, this);
	}
	m_physicActor->Init();

	GetHost().m_OnAwake.Unbind(&SphereCollider::Initialize, this);

	SetEntityScale(m_transform->GetGlobalScale());
}

void SphereCollider::InvalidateTransform()
{
	m_transform = nullptr;
}

void SphereCollider::Unregister()
{
	if (m_isRegistered)
	{
		PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

		physicManager->Unregister(this);
		m_isRegistered = false;
	}
}

void SphereCollider::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetProperty("Enabled", &isEnabled);
	m_metadatas.SetProperty("Is Trigger", &isTrigger);
	m_metadatas.SetProperty("Local Position", &localPosition);
	m_metadatas.SetProperty("Scale", &editableScale);
	m_metadatas.SetProperty("Contact Offset", &contactOffset);
}

void SphereCollider::SetEntityScale(const CCMaths::Vector3& s)
{
	CCMaths::Vector3 scale = m_transform->GetGlobalScale();
	m_entityScale = CCMaths::Max(CCMaths::Max(scale.x, scale.y), scale.z);

	m_totalScale = m_baseEntityScale * m_editableScale * m_entityScale;
	
	ComputeModelMatrices();
}

void SphereCollider::SetPxShape()
{
	m_pxShape = m_physicActor->CreateShape(physx::PxSphereGeometry(m_totalScale));
	SetPxLocalPos();

	m_pxShape->userData = this;

	SetPxData();
}

void SphereCollider::ClearPxShape()
{
	if (m_pxShape)
	{
		m_physicActor->RemoveShape(m_pxShape);
		m_pxShape = nullptr;
	}
}

void SphereCollider::ResetPxShape()
{
	if (!m_pxShape)
		return;

	ClearPxShape();
	SetPxShape();
}

void SphereCollider::SetPxData()
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

void SphereCollider::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->SubscribeToPipeline<ColliderRenderPass>(dynamic_cast<Collider*>(this));
}

void SphereCollider::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->UnsubscribeToPipeline<ColliderRenderPass>(dynamic_cast<Collider*>(this));
}

void SphereCollider::SetScale(const float& scale)
{
	m_editableScale = scale;

	m_totalScale = m_baseEntityScale * m_editableScale * m_entityScale;
	ComputeModelMatrices();

	ResetPxShape();
}

void SphereCollider::ComputeModelMatrices()
{
	if (!m_transform)
		return;

	m_model = m_transform->GetWorldMatrix().NormalizedScale() *
			  CCMaths::Matrix4::Translate(m_localPosition) *
			  CCMaths::Matrix4::Scale(m_totalScale);
}

CCMaths::Matrix4 SphereCollider::GetModelMatrix()
{
	return m_model;
}
