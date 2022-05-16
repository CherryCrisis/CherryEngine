#include "pch.hpp"

#include "box_collider.hpp"

#include <PxPhysicsAPI.h>

#include "resource_manager.hpp"
#include "physic_manager.hpp"

#include "collider_renderpass.hpp"
#include "camera_component.hpp"
#include "transform.hpp"
#include "mesh.hpp"

BoxCollider::BoxCollider()
{
	PopulateMetadatas();

	m_type = EColliderType::BOX;

	Camera* cam = CameraComponent::m_editorCamera;
	if (!cam)
		return;

	SubscribeToPipeline(cam->m_pipeline.get());
}

BoxCollider::BoxCollider(CCUUID& id) : Collider(id)
{
	PopulateMetadatas();

	m_type = EColliderType::BOX;

	Camera* cam = CameraComponent::m_editorCamera;
	if (!cam)
		return;

	SubscribeToPipeline(cam->m_pipeline.get());
}

BoxCollider::~BoxCollider()
{
	Unregister();

	Camera* cam = CameraComponent::m_editorCamera;
	if (!cam)
		return;

	//UnsubscribeToPipeline(cam->m_pipeline.get());

	if (m_transform)
	{
		m_transform->m_onScaleChange.Unbind(&BoxCollider::SetEntityScale, this);
		m_transform->m_OnDestroy.Unbind(&BoxCollider::InvalidateTransform, this);
	}
}

void BoxCollider::BindToSignals()
{
	PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

	physicManager->Register(this);
	m_isRegistered = true;

	GetHost().m_OnAwake.Bind(&BoxCollider::Initialize, this);
}

void BoxCollider::Initialize()
{
	m_transform = GetHost().GetOrAddBehaviour<Transform>();

	if (m_transform)
	{
		m_transform->m_onScaleChange.Bind(&BoxCollider::SetEntityScale, this);
		m_transform->m_OnDestroy.Bind(&BoxCollider::InvalidateTransform, this);
	}

	GetHost().m_OnAwake.Unbind(&BoxCollider::Initialize, this);

	SetEntityScale(m_transform->GetGlobalScale());
}

void BoxCollider::InvalidateTransform()
{
	m_transform = nullptr;
}

void BoxCollider::Unregister()
{
	if (m_isRegistered)
	{
		PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

		physicManager->Unregister(this);
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
	m_metadatas.SetProperty("Contact Offset", &contactOffset);
}

void BoxCollider::SetEntityScale(const CCMaths::Vector3& scale)
{
	m_entityScale = m_transform->GetGlobalScale();

	m_totalScale = m_baseEntityScale;
	m_totalScale *= m_editableScale;
	m_totalScale *= m_entityScale;
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
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, false);
		}
		else
		{
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eTRIGGER_SHAPE, false);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSIMULATION_SHAPE, true);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, true);
		}
	}
}

void BoxCollider::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->SubscribeToPipeline<ColliderRenderPass>(dynamic_cast<Collider*>(this));
}

void BoxCollider::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->UnsubscribeToPipeline<ColliderRenderPass>(dynamic_cast<Collider*>(this));
}

void BoxCollider::SetScale(const CCMaths::Vector3& scale)
{
	m_editableScale = scale;

	m_totalScale = m_baseEntityScale;
	m_totalScale *= m_editableScale;
	m_totalScale *= m_entityScale;
	
	ResetPxShape();
}

CCMaths::Matrix4 BoxCollider::GetTranformMatrix()
{
	return m_transform->GetWorldMatrix().NormalizedScale() * CCMaths::Matrix4::Translate(m_localPosition) * CCMaths::Matrix4::Scale(m_totalScale);
}