#include "pch.hpp"

#include "physic_actor.hpp"

#include <PxPhysicsAPI.h>

#include "physic_manager.hpp"
#include "collider.hpp"
#include "rigidbody.hpp"
#include "transform.hpp"
#include "cell.hpp"


namespace PhysicSystem
{
	void PhysicActor::Update()
	{
		if (m_isDynamic ||
			!m_rigidbody->GetEnabled() ||
			!m_rigidbody->GetKinematic())
		{
			Transform* t = m_owner->GetBehaviour<Transform>();
			physx::PxTransform pxT = m_pxActor->getGlobalPose();
			Vector3 pxRot = Quaternion::ToEuler({ pxT.q.w, pxT.q.x, pxT.q.y, pxT.q.z });

			Vector3 pos = t->GetPosition();
			if (pxT.p.x != m_oldPos.z)
				pos.x = pxT.p.x;

			if (pxT.p.y != m_oldPos.z)
				pos.y = pxT.p.y;

			if (pxT.p.z != m_oldPos.z)
				pos.z = pxT.p.z;

			Vector3 rot = t->GetRotation();
			if (pxRot.x != m_oldRot.x)
					rot.x = pxRot.x;

			if (pxRot.y != m_oldRot.y)
				rot.y = pxRot.y;

			if (pxRot.z != m_oldRot.z)
				rot.z = pxRot.z;

			t->SetPosition(pos);
			t->SetRotation(rot);
		}
	}

	void PhysicActor::SetActorPosition(const CCMaths::Vector3& position)
	{
		m_oldPos = position;

		physx::PxTransform pxT = m_pxActor->getGlobalPose();
		m_pxActor->setGlobalPose(physx::PxTransform(physx::PxVec3(position.x, position.y, position.z),
													pxT.q));
	}

	void PhysicActor::SetActorRotation(const CCMaths::Vector3& rotation)
	{
		m_oldRot = rotation;

		Quaternion pxRotQ = Quaternion::FromEuler(rotation);

		physx::PxTransform pxT = m_pxActor->getGlobalPose();
		m_pxActor->setGlobalPose(physx::PxTransform(pxT.p,
													physx::PxQuat(pxRotQ.x, pxRotQ.y, pxRotQ.z, pxRotQ.w)));
	}

	void PhysicActor::CreatePxActor()
	{
		Transform* t = m_owner->GetBehaviour<Transform>();
		t->m_onPositionChange.Bind(&PhysicActor::SetActorPosition, this);
		t->m_onRotationChange.Bind(&PhysicActor::SetActorRotation, this);

		Vector3 pos = t->GetPosition();
		Quaternion rot = Quaternion::FromEuler(t->GetRotation());

		physx::PxTransform transform(physx::PxVec3(pos.x, pos.y, pos.z), physx::PxQuat(rot.x, rot.y, rot.z, rot.w));

		physx::PxPhysics* physics = PhysicSystem::PhysicManager::GetInstance()->Get();

		if (m_rigidbody)
		{
			m_isDynamic = true;

			m_pxActor = physics->createRigidDynamic(physx::PxTransform(transform));
			m_rigidbody->SetPxActor();

			physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<physx::PxRigidDynamic*>(m_pxActor), m_density);
		}
		else if (!m_pxActor || m_isDynamic)
		{
			m_isDynamic = false;

			m_pxActor = physics->createRigidStatic(transform);
		}

		m_pxActor->userData = this;

		for (auto& collider : m_colliders)
		{
			collider->SetPxShape();
		}
	}

	void PhysicActor::DestroyPxActor()
	{
		if (m_pxActor)
		{
			Transform* t = m_owner->GetBehaviour<Transform>();
			t->m_onPositionChange.Unbind(&PhysicActor::SetActorPosition, this);
			t->m_onRotationChange.Unbind(&PhysicActor::SetActorRotation, this);
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
				m_owner->m_cell->m_physicCell->RemoveActor(this);
				CreatePxActor();
				m_owner->m_cell->m_physicCell->AddActor(this);
			}
		}
	}

	void PhysicActor::AddCollider(Collider* collider, bool isPlaying)
	{
		m_colliders.push_back(collider);

		if (isPlaying)
			CreatePxActor();
	}

	void PhysicActor::RemoveRigidbody(Rigidbody* rigidbody)
	{
		if (m_rigidbody == rigidbody)
		{
			m_rigidbody = nullptr;

			if (m_pxActor)
			{
				m_owner->m_cell->m_physicCell->RemoveActor(this);
				CreatePxActor();
				m_owner->m_cell->m_physicCell->AddActor(this);
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

	bool PhysicActor::HasRigidbody()
	{
		return m_rigidbody;
	}

	bool PhysicActor::HasColliders()
	{
		return !m_colliders.empty();
	}
}