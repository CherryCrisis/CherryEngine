#include "pch.hpp"

#include "rigidbody.hpp"

#include <PxPhysicsAPI.h>

#include "physic_scene.hpp"
#include "physic_actor.hpp"
#include "transform.hpp"

Rigidbody::Rigidbody(Entity& owner)
	: Behaviour(owner), m_owner(GetHost())
{
	PopulateMetadatas();

	PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();
	physicManager->Register(this);
}

void Rigidbody::PopulateMetadatas()
{
	m_metadatas.m_fields.push_back({ "Enabled", &m_isEnabled });

	m_metadatas.m_fields.push_back({ "Density",  &m_density });
	m_metadatas.m_fields.push_back({ "Kinematic", &m_isKinematic });
	m_metadatas.m_fields.push_back({ "Use gravity", &m_useGravity });
	m_metadatas.m_fields.push_back({ "Position constraints", &m_positionConstraints[0] });
	m_metadatas.m_fields.push_back({ "Rotation constraints", &m_rotationConstraints[0] });
	m_metadatas.m_fields.push_back({ "Max velocity", &m_maxLinearVelocity });
	m_metadatas.m_fields.push_back({ "Max angular velocity", &m_maxAngularVelocity });
	m_metadatas.m_fields.push_back({ "Max depenetration velocity", &m_maxDepenetrationVelocity });
}

void Rigidbody::SetPxActor()
{
	SetActorConstraints();
	SetActorEnabled();
	SetActorKinetic();
	SetActorGravity();
	SetActorMaxVelocities();
	SetActorDensity();
}

void Rigidbody::SetActorConstraints()
{
	physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(m_physicActor->Get());

	actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_X, false);
	actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Y, false);
	actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Z, false);
	actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X, false);
	actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y, false);
	actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z, false);

	for (int i = 0; i < 3; ++i)
	{
		if (m_positionConstraints[i])
			actor->setRigidDynamicLockFlag((physx::PxRigidDynamicLockFlag::Enum)(2 ^ i), true);
	}

	for (int i = 0; i < 3; ++i)
	{
		if (m_rotationConstraints[i])
			actor->setRigidDynamicLockFlag((physx::PxRigidDynamicLockFlag::Enum)(2 ^ (i + 3)), true);
	}
}

void Rigidbody::SetActorEnabled()
{
	physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(m_physicActor->Get());
	
	if (!m_isEnabled)
		actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_SIMULATION, true);
	else
		actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_SIMULATION, false);
}

void Rigidbody::SetActorKinetic()
{
	physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(m_physicActor->Get());

	if (m_isKinematic)
	{
		actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, true);
		actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, true);
	}
	else
	{
		actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, false);
		actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, false);
	}
}

void Rigidbody::SetActorGravity()
{
	physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(m_physicActor->Get());

	if (!m_useGravity)
		actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_GRAVITY, true);
	else
		actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_GRAVITY, false);
}

void Rigidbody::SetActorMaxVelocities()
{
	physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(m_physicActor->Get());

	actor->setMaxLinearVelocity(m_maxLinearVelocity);

	actor->setMaxAngularVelocity(m_maxAngularVelocity);

	actor->setMaxDepenetrationVelocity(m_maxDepenetrationVelocity);
}

void Rigidbody::SetActorDensity()
{
	m_physicActor->SetDensity(m_density);
}