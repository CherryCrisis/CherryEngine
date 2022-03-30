#include "pch.hpp"

#include "physic_actor.hpp"

#include <PxPhysicsAPI.h>

#include "physic_manager.hpp"
#include "collider.hpp"
#include "rigidbody.hpp"
#include "transform.hpp"


namespace PhysicSystem
{
	void PhysicActor::Update()
	{
		physx::PxTransform pxT = m_pxActor->getGlobalPose();

		Transform* t = m_owner->GetBehaviour<Transform>();

		t->SetPosition({ pxT.p.x, pxT.p.y, pxT.p.z });
		t->SetRotation(Quaternion::ToEuler({ pxT.q.w, pxT.q.x, pxT.q.y, pxT.q.z }));
	}

	// TODO: Add actor in Scene according to owner cell

	// TODO: Call on play
	void PhysicActor::CreatePxActor()
	{
		Transform* t = m_owner->GetBehaviour<Transform>();
		Vector3 pos = t->GetPosition();
		Quaternion rot = Quaternion::FromEuler(t->GetRotation());

		physx::PxTransform transform(physx::PxVec3(pos.x, pos.y, pos.z), physx::PxQuat(rot.x, rot.y, rot.z, rot.w));

		physx::PxPhysics* physics = PhysicSystem::PhysicManager::GetInstance()->Get();

		if (m_rigidbody)
		{
			m_isDynamic = true;

			m_pxActor = physics->createRigidDynamic(transform);
			m_rigidbody->SetPxActor();

			physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<physx::PxRigidDynamic*>(m_pxActor), m_density);
		}
		else
		{
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
		PX_RELEASE(m_pxActor);
	}

	physx::PxShape* PhysicActor::CreateShape(const physx::PxGeometry& geometry)
	{
		PhysicManager* physicManager = PhysicManager::GetInstance();

		return physx::PxRigidActorExt::createExclusiveShape(*m_pxActor, geometry, *physicManager->GetMaterial());
	}

	void PhysicActor::RemoveShape(physx::PxShape* shape)
	{
		m_pxActor->detachShape(*shape);
	}

	void PhysicActor::AddRigidbody(Rigidbody* rigidbody)
	{
		if (!m_rigidbody)
			m_rigidbody = rigidbody;
	}

	void PhysicActor::AddCollider(Collider* collider)
	{
		m_colliders.push_back(collider);
	}

	void PhysicActor::RemoveRigidbody(Rigidbody* rigidbody)
	{
		if (m_rigidbody == rigidbody)
			m_rigidbody = nullptr;
	}

	void PhysicActor::RemoveCollider(Collider* collider)
	{
		for (size_t i = 0; i < m_colliders.size(); ++i)
		{
			if (collider == m_colliders[i])
			{
				m_colliders[i] = m_colliders.back();
				m_colliders.pop_back();
			}
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