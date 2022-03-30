#include "pch.hpp"

#include "collider.hpp"

#include <PxPhysicsAPI.h>

#include "physic_scene.hpp"
#include "physic_actor.hpp"
#include "transform.hpp"

Collider::Collider(Entity& owner)
	: Behaviour(owner), m_owner(GetHost())
{
	PopulateMetadatas();

	PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();
	physicManager->Register(this);
}

void Collider::PopulateMetadatas()
{/*
	m_metadatas.m_fields.push_back({ "Enabled", &m_isEnabled});
	m_metadatas.m_fields.push_back({ "Is Trigger", &m_isTrigger});
	m_metadatas.m_fields.push_back({ "Shape", &m_colliderShape});
	m_metadatas.m_fields.push_back({ "Contact Offset", &m_contactOffset});
	*/
}

void Collider::SetPxShape()
{
	if (m_pxShape)
	{
		m_physicActor->RemoveShape(m_pxShape);
		m_pxShape = nullptr;
	}

	if (!m_pxShape)
	{
		switch (m_colliderShape)
		{
		case ColliderShape::NONE:
			break;
		case ColliderShape::BOX_COLLIDER:
			m_pxShape = m_physicActor->CreateShape(physx::PxBoxGeometry(physx::PxVec3(1.f, 1.f, 1.f)));
			break;
		case ColliderShape::SPHERE_COLLIDER:
			m_pxShape = m_physicActor->CreateShape(physx::PxSphereGeometry(1.f));
			break;
		default:
			break;
		}
	}
	// TODO: Give appropriate size to colliders
}

void Collider::SetPxData()
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
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSIMULATION_SHAPE, true);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eTRIGGER_SHAPE, false);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, true);
		}
	}
}
