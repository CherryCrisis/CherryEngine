#pragma once

#include "cherry_macros.hpp"

#include "behaviour.hpp"
#include "physic_manager.hpp"

class Rigidbody;
class Collider;

namespace PhysicSystem
{
	class CCENGINE_API PhysicActor
	{
	protected:
		physx::PxRigidActor* m_pxActor = nullptr;

		bool	m_isDynamic = false;
		float	m_density = 0.0f;

		Rigidbody* m_rigidbody = nullptr;
		std::vector<Collider*> m_colliders;

	public:
		Entity* m_owner = nullptr;

		void Update();
		
		void CreatePxActor();
		void DestroyPxActor();
		
		physx::PxShape* CreateShape(const physx::PxGeometry& geometry);
		void			RemoveShape(physx::PxShape* shape);

		void AddRigidbody(Rigidbody* rigidbody);
		void AddCollider(Collider* collider);

		void RemoveRigidbody(Rigidbody* rigidbody);
		void RemoveCollider(Collider* collider);

		void SetDensity(float density) { m_density = density; }

		physx::PxRigidActor* Get() { return m_pxActor; }
		
		bool HasRigidbody();
		bool HasColliders();
	};
}