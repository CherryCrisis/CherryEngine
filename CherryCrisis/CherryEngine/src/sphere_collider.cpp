#include "pch.hpp"

#include "sphere_collider.hpp"

#include <PxPhysicsAPI.h>

#include "physic_manager.hpp"
#include "transform.hpp"

SphereCollider::SphereCollider()
{
	PopulateMetadatas();
}

SphereCollider::SphereCollider(CCUUID& id) : Collider(id)
{
	PopulateMetadatas();
}

SphereCollider::~SphereCollider()
{
	Unregister();
}

void SphereCollider::BindToSignals()
{
	PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

	physicManager->Register(this);
	m_isRegistered = true;

	Transform* t = m_physicActor->m_owner->GetBehaviour<Transform>();
	SetEntityScale(t->GetScale());
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
	m_metadatas.SetProperty("Scale", &editableScale);
	m_metadatas.SetProperty("Contact Offset", &contactOffset);
}

void SphereCollider::SetEntityScale(const CCMaths::Vector3& scale)
{
	m_entityScale = scale.Length();
}

void SphereCollider::SetPxShape()
{
	if (m_pxShape)
		return;

	float scale = m_baseEntityScale * m_editableScale * m_entityScale;
	m_pxShape = m_physicActor->CreateShape(physx::PxSphereGeometry(scale));
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
