#include "pch.hpp"

#include "capsule_collider.hpp"

#include <PxPhysicsAPI.h>

#include "physic_manager.hpp"
#include "transform.hpp"
#include "entity.hpp"

CapsuleCollider::CapsuleCollider()
{
	PopulateMetadatas();
}

CapsuleCollider::CapsuleCollider(CCUUID& id) : Collider(id)
{
	PopulateMetadatas();
}

CapsuleCollider::~CapsuleCollider()
{
	Unregister();
}

void CapsuleCollider::BindToSignals()
{
	PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

	physicManager->Register(this);
	m_isRegistered = true;

	Transform* t = m_physicActor->m_owner->GetOrAddBehaviour<Transform>();
	SetEntityScale(t->GetScale());
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

void CapsuleCollider::SetEntityScale(const CCMaths::Vector3& scale)
{
	m_entityRadius = CCMaths::Min(scale.x, scale.z);
	m_entityScale = CCMaths::Max(0.01f, (scale.y - m_entityRadius));
}

void CapsuleCollider::SetPxShape()
{
	float scale = m_editableScale * m_entityScale;
	float totalRadius = m_editableRadius * m_entityRadius;

	m_pxShape = m_physicActor->CreateShape(physx::PxCapsuleGeometry(totalRadius, scale));

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