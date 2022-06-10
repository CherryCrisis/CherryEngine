#include <pch.hpp>

#include "physic_actor.hpp"

#include <PxPhysicsAPI.h>

#include "debug.hpp"
#include "physic_manager.hpp"

#include "capsule_collider.hpp"
#include "cell.hpp"
#include "character_controller.hpp"
#include "entity.hpp"
#include "rigidbody.hpp"
#include "transform.hpp"


namespace PhysicSystem
{
	PhysicActor::~PhysicActor()
	{
		m_owner->m_cell->RemoveEntityFromPhysicScene(m_owner);

		if (m_pxActor)
		{
			m_pxActor->userData = nullptr;
			DestroyPxActor();
		}

		if (m_transform)
		{
			m_transform->m_onPositionChange.Unbind(&PhysicActor::SetActorPosition, this);
			m_transform->m_onRotationChange.Unbind(&PhysicActor::SetActorRotation, this);
			m_transform->m_onScaleChange.Unbind(&PhysicActor::SetActorScale, this);
			m_transform->m_OnDestroyed.Unbind(&PhysicActor::InvalidateTransform, this);
		}
	}

	void PhysicActor::Init()
	{
		// Transform already set means Init() has already been called by another physic related component
		if (m_transform)
		{
			if (PhysicManager::IsPlaying())
			{
				DestroyPxActor();
				CreatePxActor();
			}

			return;
		}
		
		// Else set transform
		m_transform = m_owner->GetOrAddBehaviour<Transform>();

		if (m_transform)
		{
			m_transform->m_onPositionChange.Bind(&PhysicActor::SetActorPosition, this);
			m_transform->m_onRotationChange.Bind(&PhysicActor::SetActorRotation, this);
			m_transform->m_onScaleChange.Bind(&PhysicActor::SetActorScale, this);
			m_transform->m_OnDestroyed.Bind(&PhysicActor::InvalidateTransform, this);

			if (PhysicManager::IsPlaying())
				CreatePxActor();
		}
	}

	void PhysicActor::InvalidateTransform()
	{
		m_transform = nullptr;
	}

	void PhysicActor::Update()
	{
		if (m_isDynamic)
		{
			if (!m_rigidbody->GetEnabled() ||
				m_rigidbody->GetKinematic())
				return;
			
			Transform* t = m_transform;
			if (!t)
				return;
			
			physx::PxTransform pxT = m_pxActor->getGlobalPose();
			Vector3 pxPos = { pxT.p.x, pxT.p.y, pxT.p.z };
			Quaternion pxRot = { pxT.q.w, pxT.q.x, pxT.q.y, pxT.q.z };

			if (pxPos != m_oldPos)
			{
				m_oldPos = pxPos;
				m_dontResetPhysx = true;
				t->SetGlobalPosition(pxPos);
			}

			if (pxRot != m_oldRot)
			{
				m_oldRot = pxRot;
				m_dontResetPhysx = true;
				t->SetGlobalRotation(Quaternion::ToEuler(pxRot));
			}
		}
	}

	void PhysicActor::SetActorPosition(Transform* transform)
	{
		if (m_pxActor && !m_dontResetPhysx)
		{
			Vector3 pos = transform->GetGlobalPosition();
			physx::PxTransform pxT = m_pxActor->getGlobalPose();
			m_pxActor->setGlobalPose(physx::PxTransform(physx::PxVec3(pos.x, pos.y, pos.z),
				pxT.q));
			m_oldPos = pos;
		}
		else
			m_dontResetPhysx = false;
	}

	void PhysicActor::SetActorRotation(Transform* transform)
	{
		if (m_pxActor && !m_dontResetPhysx)
		{
			Quaternion rot = transform->GetGlobalRotation();
			physx::PxTransform pxT = m_pxActor->getGlobalPose();
			m_pxActor->setGlobalPose(physx::PxTransform(pxT.p,
				physx::PxQuat(rot.x, rot.y, rot.z, rot.w)));
			m_oldRot = rot;
		}
		else
			m_dontResetPhysx = false;
	}

	void PhysicActor::SetActorScale(Transform* transform)
	{
		for (auto& collider : m_colliders)
		{
			collider->SetEntityScale(transform);
			collider->ResetPxShape();
		}
	}

	void PhysicActor::CreatePxActor()
	{
		if (!m_transform)
			return;

		Vector3 pos = m_transform->GetGlobalPosition();
		Quaternion rot = m_transform->GetGlobalRotation();

		physx::PxTransform transform(physx::PxVec3(pos.x, pos.y, pos.z), physx::PxQuat(rot.x, rot.y, rot.z, rot.w));

		physx::PxPhysics* physics = PhysicSystem::PhysicManager::Get();

		if (m_rigidbody)
		{
			m_isDynamic = true;

			m_pxActor = physics->createRigidDynamic(physx::PxTransform(transform));
		
			physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(Get());

			actor->setLinearDamping(0.2f);
			actor->setAngularDamping(0.2f);
		}
		else
		{
			m_isDynamic = false;
			m_isStatic = true;

			m_pxActor = physics->createRigidStatic(transform);
		}

		for (auto& collider : m_colliders)
		{
			collider->SetPxShape();
		}

		m_pxActor->userData = this;
		SetPxActor();

		m_owner->m_cell->m_physicCell->AddPxActor(this);
	}

	void PhysicActor::DestroyPxActor()
	{
		if (m_pxActor)
		{
			m_owner->m_cell->m_physicCell->RemovePxActor(this);

			PX_RELEASE(m_pxActor);
		}
	}

	physx::PxShape* PhysicActor::CreateShape(const physx::PxGeometry& geometry)
	{
		return physx::PxRigidActorExt::createExclusiveShape(*m_pxActor, geometry, *PhysicManager::GetMaterial());
	}

	void PhysicActor::RemoveShape(physx::PxShape* shape)
	{
		if (m_pxActor)
			m_pxActor->detachShape(*shape);
	}

	void PhysicActor::AddRigidbody(Rigidbody* rigidbody)
	{
		if (!m_rigidbody)
		{
			m_rigidbody = rigidbody;
			if (m_pxActor)
			{
				DestroyPxActor();
				CreatePxActor();
			}
		}
	}

	void PhysicActor::AddCollider(Collider* collider)
	{
		for (auto& coll : m_colliders)
			if (collider == coll)
				return;

		m_colliders.push_back(collider);

		if (m_pxActor)
		{
			DestroyPxActor();
			CreatePxActor();
		}
	}

	void PhysicActor::AddController(CharacterController* controller)
	{
		if (!m_controller)
		{
			m_controller = controller;
		}
	}

	void PhysicActor::RemoveRigidbody(Rigidbody* rigidbody)
	{
		if (m_controller)
		{
			if (rigidbody == m_controller->m_rigidbody)
				m_controller->m_rigidbody = nullptr;
		}

		if (m_rigidbody == rigidbody)
		{
			m_rigidbody = nullptr;

			if (m_pxActor)
			{
				DestroyPxActor();
				CreatePxActor();
			}
		}
	}

	void PhysicActor::RemoveCollider(Collider* collider)
	{
		if (m_controller)
		{
			if (collider == m_controller->m_collider)
				m_controller->m_collider = nullptr;
		}

		for (size_t i = 0; i < m_colliders.size(); ++i)
		{
			if (collider == m_colliders[i])
			{
				if (m_pxActor)
					m_colliders[i]->ClearPxShape();

				m_colliders[i] = m_colliders.back();
				m_colliders.pop_back();
			}
		}
	}

	void PhysicActor::RemoveController(CharacterController* controller)
	{
		if (m_pxActor)
			return;
		
		if (m_controller == controller)
		{
			m_controller = nullptr;
		}
	}

	void PhysicActor::Empty()
	{
		if (m_rigidbody)
			m_rigidbody->Unregister(false);

		if (m_controller)
			m_controller->Unregister(false);

		while (!m_colliders.empty())
		{
			m_colliders.back()->Unregister(false);
		}
	}

	void PhysicActor::AddForce(const CCMaths::Vector3& force, EForceMode mode)
	{
		if (m_pxActor && m_isDynamic)
		{
			physx::PxVec3 pxForce = { force.x, force.y, force.z };
			physx::PxRigidDynamic* object = static_cast<physx::PxRigidDynamic*>(m_pxActor);
			object->addForce(pxForce, static_cast<physx::PxForceMode::Enum>(mode), true);
		}
	}

	void PhysicActor::AddTorque(const CCMaths::Vector3& force, EForceMode mode)
	{
		if (m_pxActor && m_isDynamic)
		{
			physx::PxVec3 pxForce = { force.x, force.y, force.z };
			physx::PxRigidDynamic* object = static_cast<physx::PxRigidDynamic*>(m_pxActor);
			object->addTorque(pxForce, static_cast<physx::PxForceMode::Enum>(mode), true);
		}
	}

	RaycastHit PhysicActor::Raycast(const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange)
	{
		physx::PxRaycastBuffer hit = PhysicManager::RaycastBuff(*m_owner->m_cell->m_physicCell, origin, dir, maxRange);

		unsigned int hitNb = (unsigned int)hit.getNbAnyHits();

		if (hitNb == 0)
		{
			return RaycastHit();
		}

		for (int i = hitNb - 1; i >= 0; i--)
		{
			if (hit.getAnyHit(i).actor != m_pxActor)
				return PhysicManager::RaycastFromPxRaycast(hit.getAnyHit(i));
		}
		return RaycastHit();
	}

	bool PhysicActor::HasRigidbody()
	{
		return m_rigidbody;
	}

	bool PhysicActor::HasController()
	{
		return m_controller;
	}

	bool PhysicActor::HasColliders()
	{
		return !m_colliders.empty();
	}


	void PhysicActor::SetRaycastDir(const CCMaths::Vector3& dir)
	{
		if (m_controller)
			m_controller->SetRaycastDir(dir.Normalized());
	}

	void PhysicActor::SetPxActor()
	{
		SetActorEnabled();
		SetActorDensity();
		SetActorConstraints();
		SetActorKinematic();
		SetActorGravity();
		SetActorMaxVelocities();
	}

	void PhysicActor::SetActorConstraints()
	{
		if (!m_isDynamic)
			return;

		physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(Get());

		if (actor)
		{
			Bool3 posCons = m_rigidbody->GetPosConstraints();
			Bool3 rotCons = m_rigidbody->GetRotConstraints();

			actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_X, posCons.x);
			actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Y, posCons.y);
			actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Z, posCons.z);
			actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X, rotCons.x);
			actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y, rotCons.y);
			actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z, rotCons.z);

			if (m_isEnabled && actor->getScene() && !m_rigidbody->GetKinematic())
				actor->wakeUp();
		}
	}

	void PhysicActor::SetActorEnabled()
	{
		if (!m_isDynamic)
			return;

		physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(Get());

		if (actor)
		{
			if (!m_isEnabled)
				actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_SIMULATION, true);
			else
				actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_SIMULATION, false);

			if (m_isEnabled && actor->getScene() && !m_rigidbody->GetKinematic())
				actor->wakeUp();
		}
	}

	void PhysicActor::SetActorKinematic()
	{
		if (m_isStatic)
			return;

		physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(Get());
		
		if (!m_isDynamic)
		{
			actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, true);
			actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, true);
			return;
		}

		if (actor)
		{
			if (m_rigidbody->GetKinematic())
			{
				actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, true);
				actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, true);
			}
			else
			{
				actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, false);
				actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, false);
			}


			if (m_isEnabled && actor->getScene() && !m_rigidbody->GetKinematic())
				actor->wakeUp();
		}
	}

	void PhysicActor::SetActorGravity()
	{
		if (!m_isDynamic)
			return;

		physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(Get());

		if (actor)
		{
			if (m_rigidbody->GetGravity())
				actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_GRAVITY, false);
			else
				actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_GRAVITY, true);


			if (m_isEnabled && actor->getScene() && !m_rigidbody->GetKinematic())
				actor->wakeUp();
		}
	}

	void PhysicActor::SetActorDensity()
	{
		if (!m_isDynamic)
			return;

		physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(Get());

		if (actor)
			physx::PxRigidBodyExt::updateMassAndInertia(*actor, m_rigidbody->GetDensity());
	}

	void PhysicActor::SetActorMaxVelocities()
	{
		if (!m_isDynamic)
			return;

		physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(Get());

		if (actor)
		{
			actor->setMaxLinearVelocity(m_rigidbody->GetMaxVel());

			actor->setMaxAngularVelocity(m_rigidbody->GetMaxAngVel());

			actor->setMaxDepenetrationVelocity(m_rigidbody->GetMaxDepVel());
		}
	}
}
