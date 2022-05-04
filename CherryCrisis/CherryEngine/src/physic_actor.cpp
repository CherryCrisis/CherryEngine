#include "pch.hpp"

#include "physic_actor.hpp"

#include <PxPhysicsAPI.h>

#include "physic_manager.hpp"
#include "collider.hpp"
#include "rigidbody.hpp"
#include "transform.hpp"
#include "cell.hpp"

#include "debug.hpp"

namespace PhysicSystem
{
	void PhysicActor::Update()
	{
		if (m_isDynamic)
		{
			if (!m_rigidbody->GetEnabled() ||
				GetKinematic())
				return;
			
			Transform* t = m_owner->GetBehaviour<Transform>();
			physx::PxTransform pxT = m_pxActor->getGlobalPose();
			Vector3 pxRot = Quaternion::ToEuler({ pxT.q.w, pxT.q.y, pxT.q.x, pxT.q.z });

			// TODO: Change with world tranform
			Vector3 pos = t->GetPosition();
			pos.x = pxT.p.x != m_oldPos.x ? pxT.p.x : pos.x;
			pos.y = pxT.p.y != m_oldPos.y ? pxT.p.y : pos.y;
			pos.z = pxT.p.z != m_oldPos.z ? pxT.p.z : pos.z;

			Vector3 rot = t->GetRotation();
			rot.x = pxRot.x != m_oldRot.x ? pxRot.x : rot.x;
			rot.y = pxRot.y != m_oldRot.y ? pxRot.y : rot.y;
			rot.z = pxRot.z != m_oldRot.z ? pxRot.z : rot.z;

			// TODO: Change with world tranform
			t->SetPosition(pos);
			t->SetRotation(rot);
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

		Quaternion pxRotQ = Quaternion::FromEuler(rotation);


		if (m_pxActor)
		{
			physx::PxTransform pxT = m_pxActor->getGlobalPose();
			m_pxActor->setGlobalPose(physx::PxTransform(pxT.p,
				physx::PxQuat(pxRotQ.y, pxRotQ.x, pxRotQ.z, pxRotQ.w)));
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
		Transform* t = m_owner->GetBehaviour<Transform>();
		t->m_onPositionChange.Bind(&PhysicActor::SetActorPosition, this);
		t->m_onRotationChange.Bind(&PhysicActor::SetActorRotation, this);
		t->m_onScaleChange.Bind(&PhysicActor::SetActorScale, this);

		Vector3 pos = t->GetPosition();
		Quaternion rot = Quaternion::FromEuler(t->GetRotation());

		physx::PxTransform transform(physx::PxVec3(pos.x, pos.y, pos.z), physx::PxQuat(rot.y, rot.x, rot.z, rot.w));

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
	}

	void PhysicActor::DestroyPxActor()
	{
		if (m_pxActor)
		{
			m_owner->m_cell->m_physicCell->RemovePxActor(this);

			Transform* t = m_owner->GetBehaviour<Transform>();
			t->m_onPositionChange.Unbind(&PhysicActor::SetActorPosition, this);
			t->m_onRotationChange.Unbind(&PhysicActor::SetActorRotation, this);
			t->m_onScaleChange.Unbind(&PhysicActor::SetActorScale, this);
			PX_RELEASE(m_pxActor);
		}
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

	void PhysicActor::RemoveRigidbody(Rigidbody* rigidbody)
	{
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
			object->addForce(pxForce, mode, true);
		}
	}

	RaycastHit PhysicActor::Raycast(const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange)
	{
		RaycastHit firstHit = PhysicManager::RaycastInScene(*m_owner->m_cell->m_physicCell, origin, dir, maxRange);

		if (firstHit.actor == static_cast<physx::PxRigidActor*>(m_pxActor))
		{
			CCMaths::Vector3 newOrigin = { firstHit.position.x, firstHit.position.y, firstHit.position.z };
			RaycastHit secondHit = PhysicManager::RaycastInScene(*m_owner->m_cell->m_physicCell, newOrigin, dir, maxRange - firstHit.distance);
		}
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