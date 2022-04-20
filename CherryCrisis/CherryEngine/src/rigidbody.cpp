#include "pch.hpp"

#include "rigidbody.hpp"

#include <PxPhysicsAPI.h>

#include "physic_manager.hpp"
#include "transform.hpp"

Rigidbody::Rigidbody()
{
	PopulateMetadatas();
}

Rigidbody::~Rigidbody()
{
	Unregister();
}

void Rigidbody::BindToSignals()
{
	PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();
	
	physicManager->Register(this);
	m_isRegistered = true;
}

void Rigidbody::Unregister()
{
	if (m_isRegistered)
	{
		PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

		physicManager->Unregister(this);
		m_isRegistered = false;
	}
}

void Rigidbody::PopulateMetadatas()
{
	m_metadatas.SetProperty("Density", &density);
	m_metadatas.SetProperty("Enabled", &isEnabled);
	m_metadatas.SetProperty("Kinematic", &isKinematic);
	m_metadatas.SetProperty("Use gravity", &useGravity);

	m_metadatas.SetProperty("Position constraints", &positionConstraints);
	m_metadatas.SetProperty("Rotation constraints", &rotationConstraints);

	m_metadatas.SetProperty("Max velocity", &maxLinearVelocity);
	m_metadatas.SetProperty("Max angular velocity", &maxAngularVelocity);
	m_metadatas.SetProperty("Max depenetration velocity", &maxDepenetrationVelocity);
}

void Rigidbody::SetPxActor()
{
	SetActorConstraints();
	SetActorEnabled();
	SetActorKinematic();
	SetActorGravity();
	SetActorMaxVelocities();
	SetActorDensity();
}

void Rigidbody::SetActorConstraints()
{
	if (!m_physicActor)
		return;

	physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(m_physicActor->Get());

	if (actor)
	{
		actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_X, false);
		actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Y, false);
		actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Z, false);
		actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X, false);
		actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y, false);
		actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z, false);

		for (int i = 0; i < 3; ++i)
		{
			if (m_positionConstraints.data[i])
				actor->setRigidDynamicLockFlag((physx::PxRigidDynamicLockFlag::Enum)(2 ^ i), true);
		}

		for (int i = 0; i < 3; ++i)
		{
			if (m_rotationConstraints.data[i])
				actor->setRigidDynamicLockFlag((physx::PxRigidDynamicLockFlag::Enum)(2 ^ (i + 3)), true);
		}

		actor->wakeUp();
	}
}

void Rigidbody::SetActorEnabled()
{
	if (!m_physicActor)
		return;

	physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(m_physicActor->Get());
	
	if (actor)
	{
		if (!m_isEnabled)
			actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_SIMULATION, true);
		else
			actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_SIMULATION, false);

		actor->wakeUp();
	}
}

void Rigidbody::SetActorKinematic()
{
	if (!m_physicActor)
		return;

	physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(m_physicActor->Get());

	if (actor)
	{
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

		actor->wakeUp();
	}
}

void Rigidbody::SetActorGravity()
{
	if (!m_physicActor)
		return;

	physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(m_physicActor->Get());

	if (actor)
	{
		if (!m_useGravity)
			actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_GRAVITY, true);
		else
			actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_GRAVITY, false);
		
		actor->wakeUp();
	}

}

void Rigidbody::SetActorMaxVelocities()
{
	if (!m_physicActor)
		return;

	physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(m_physicActor->Get());

	if (actor)
	{
		actor->setMaxLinearVelocity(m_maxLinearVelocity);

		actor->setMaxAngularVelocity(m_maxAngularVelocity);

		actor->setMaxDepenetrationVelocity(m_maxDepenetrationVelocity);
	}
}

void Rigidbody::SetActorDensity()
{
	m_physicActor->SetDensity(m_density);
}
