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

		m_owner->m_OnDestroyed.Unbind(&PhysicActor::UnlockDelete, this);

		if (m_transform)
		{
			m_transform->m_onPositionChange.Unbind(&PhysicActor::SetActorPosition, this);
			m_transform->m_onRotationChange.Unbind(&PhysicActor::SetActorRotation, this);
			m_transform->m_onScaleChange.Unbind(&PhysicActor::SetActorScale, this);
			m_transform->m_OnDestroy.Unbind(&PhysicActor::InvalidateTransform, this);
		}
	}

	void PhysicActor::Init()
	{
		if (m_transform)
			return;

		m_transform = m_owner->GetOrAddBehaviour<Transform>();
		m_owner->m_OnDestroyed.Bind(&PhysicActor::UnlockDelete, this);

		if (m_transform)
		{
			m_transform->m_onPositionChange.Bind(&PhysicActor::SetActorPosition, this);
			m_transform->m_onRotationChange.Bind(&PhysicActor::SetActorRotation, this);
			m_transform->m_onScaleChange.Bind(&PhysicActor::SetActorScale, this);
			m_transform->m_OnDestroy.Bind(&PhysicActor::InvalidateTransform, this);
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
				GetKinematic())
				return;
			
			Transform* t = m_owner->GetBehaviour<Transform>();
			physx::PxTransform pxT = m_pxActor->getGlobalPose();
			Vector3 pxRot = Quaternion::ToEuler({ pxT.q.w, pxT.q.x, pxT.q.y, pxT.q.z });

			Vector3 pos = t ? t->GetPosition() : Vector3::Zero;
			pos.x = pxT.p.x != m_oldPos.x ? pxT.p.x : pos.x;
			pos.y = pxT.p.y != m_oldPos.y ? pxT.p.y : pos.y;
			pos.z = pxT.p.z != m_oldPos.z ? pxT.p.z : pos.z;

			Vector3 rot = t ? t->GetRotation() : Vector3::Zero;
			rot.x = pxRot.x != m_oldRot.x ? pxRot.x : rot.x;
			rot.y = pxRot.y != m_oldRot.y ? pxRot.y : rot.y;
			rot.z = pxRot.z != m_oldRot.z ? pxRot.z : rot.z;

			if (t)
			{
				t->SetPosition(pos);
				t->SetRotation(rot);
			}

			if (m_controller)
				m_controller->Update();
		}
	}

	void PhysicActor::SetActorPosition(const CCMaths::Vector3& position)
	{
		m_oldPos = position;

		if (m_pxActor)
		{
			physx::PxTransform pxT = m_pxActor->getGlobalPose();
			m_pxActor->setGlobalPose(physx::PxTransform(physx::PxVec3(position.x, position.y, position.z),
														pxT.q));
		}
	}

	void PhysicActor::SetActorRotation(const CCMaths::Vector3& rotation)
	{
		m_oldRot = rotation;



		if (m_pxActor)
		{
			Quaternion pxRotQ = Quaternion::FromEuler(rotation);
			
			physx::PxTransform pxT = m_pxActor->getGlobalPose();
			m_pxActor->setGlobalPose(physx::PxTransform(pxT.p,
				physx::PxQuat(pxRotQ.x, pxRotQ.y, pxRotQ.z, pxRotQ.w)));
		}
	}

	void PhysicActor::SetActorScale(const CCMaths::Vector3& scale)
	{
		for (auto& collider : m_colliders)
		{
			collider->SetEntityScale(scale);
			collider->ResetPxShape();
		}
	}

	void PhysicActor::CreatePxActor()
	{
		if (!m_transform)
			return;

		Vector3 pos = m_transform->GetPosition();
		Quaternion rot = Quaternion::FromEuler(m_transform->GetRotation());

		physx::PxTransform transform(physx::PxVec3(pos.x, pos.y, pos.z), physx::PxQuat(rot.x, rot.y, rot.z, rot.w));

		physx::PxPhysics* physics = PhysicSystem::PhysicManager::GetInstance()->Get();

		if (m_isStatic)
		{
			m_isDynamic = false;

			m_pxActor = physics->createRigidStatic(transform);
		}
		else if (m_rigidbody)
		{
			m_isDynamic = true;

			m_pxActor = physics->createRigidDynamic(physx::PxTransform(transform));
			SetPxActor();

			physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<physx::PxRigidDynamic*>(m_pxActor), m_density);
		}
		else if (!m_pxActor || m_isDynamic)
		{
			m_isDynamic = false;
			m_isKinematic = true;

			m_pxActor = physics->createRigidDynamic(physx::PxTransform(transform));
			SetPxActor();

			physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<physx::PxRigidDynamic*>(m_pxActor), m_density);
		}

		m_pxActor->userData = this;

		for (auto& collider : m_colliders)
		{
			collider->SetPxShape();
		}

		m_owner->m_cell->m_physicCell->AddPxActor(this);

		m_isRemoveLocked++;
	}

	void PhysicActor::DestroyPxActor()
	{
		if (m_pxActor)
		{
			m_owner->m_cell->m_physicCell->RemovePxActor(this);

			PX_RELEASE(m_pxActor);
		}
	}

	void PhysicActor::UnlockDelete()
	{
		m_isRemoveLocked = 0;
	}

	physx::PxShape* PhysicActor::CreateShape(const physx::PxGeometry& geometry)
	{
		PhysicManager* physicManager = PhysicManager::GetInstance();

		return physx::PxRigidActorExt::createExclusiveShape(*m_pxActor, geometry, *physicManager->GetMaterial());
	}

	void PhysicActor::RemoveShape(physx::PxShape* shape)
	{
		if (m_pxActor)
			m_pxActor->detachShape(*shape);
	}

	void PhysicActor::AddRigidbody(Rigidbody* rigidbody, bool isPlaying)
	{
		if (!m_rigidbody)
		{
			m_rigidbody = rigidbody;
			if (isPlaying)
			{
				DestroyPxActor();
				CreatePxActor();
			}
		}
	}

	void PhysicActor::AddCollider(Collider* collider, bool isPlaying)
	{
		for (auto& coll : m_colliders)
			if (collider == coll)
				return;

		m_colliders.push_back(collider);

		if (isPlaying)
		{
			DestroyPxActor();
			CreatePxActor();
		}
	}

	void PhysicActor::AddController(CharacterController* controller, bool isPlaying)
	{
		if (!m_controller)
		{
			m_controller = controller;
			m_isRemoveLocked++;
		}
	}

	void PhysicActor::RemoveRigidbody(Rigidbody* rigidbody)
	{
		if (m_isRemoveLocked > 0)
			return;

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
		if (m_isRemoveLocked > 0)
			return;

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
			m_isRemoveLocked = false;
		}
	}

	void PhysicActor::Empty()
	{
		if (m_rigidbody)
		{
			m_rigidbody->Unregister();
		}

		while (!m_colliders.empty())
		{
			m_colliders.back()->Unregister();
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

	bool PhysicActor::HasColliders()
	{
		return !m_colliders.empty();
	}


	void PhysicActor::SetPxActor()
	{
		SetActorConstraints();
		SetActorEnabled();
		SetActorKinematic();
		SetActorGravity();
		SetActorMaxVelocities();
	}

	void PhysicActor::SetActorConstraints()
	{
		physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(Get());

		if (actor)
		{
			actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_X, m_positionConstraints.x);
			actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Y, m_positionConstraints.y);
			actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Z, m_positionConstraints.z);
			actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X, m_rotationConstraints.x);
			actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y, m_rotationConstraints.y);
			actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z, m_rotationConstraints.z);

			if (actor->getScene() && !GetKinematic())
				actor->wakeUp();
		}
	}

	void PhysicActor::SetActorEnabled()
	{
		physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(Get());

		if (actor)
		{
			if (!m_isEnabled)
				actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_SIMULATION, true);
			else
				actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_SIMULATION, false);

			if (actor->getScene() && !GetKinematic())
				actor->wakeUp();
		}
	}

	void PhysicActor::SetActorKinematic()
	{
		physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(Get());

		if (actor)
		{
			if (GetKinematic())
			{
				actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, true);
				actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, true);
			}
			else
			{
				actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, false);
				actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, false);
			}


			if (actor->getScene() && !GetKinematic())
				actor->wakeUp();
		}
	}

	void PhysicActor::SetActorGravity()
	{
		physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(Get());

		if (actor)
		{
			if (!m_useGravity)
				actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_GRAVITY, true);
			else
				actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_GRAVITY, false);


			if (actor->getScene() && !GetKinematic())
				actor->wakeUp();
		}
	}

	void PhysicActor::SetActorMaxVelocities()
	{
		physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(Get());

		if (actor)
		{
			actor->setMaxLinearVelocity(m_maxLinearVelocity);

			actor->setMaxAngularVelocity(m_maxAngularVelocity);

			actor->setMaxDepenetrationVelocity(m_maxDepenetrationVelocity);
		}
	}
}