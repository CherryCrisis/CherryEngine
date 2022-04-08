#pragma once

#include "cherry_macros.hpp"

#include "behaviour.hpp"

class Rigidbody;
class Collider;

namespace physx
{
	class PxRigidActor;
	class PxGeometry;
	class PxShape;
}

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

		void AddRigidbody(Rigidbody* rigidbody, bool isPlaying = false);
		void AddCollider(Collider* collider, bool isPlaying = false);

		void RemoveRigidbody(Rigidbody* rigidbody);
		void RemoveCollider(Collider* collider);
		void Empty();

		void SetDensity(float density) { m_density = density; }

		physx::PxRigidActor* Get() { return m_pxActor; }
		
		bool HasRigidbody();
		bool HasColliders();
	};
}