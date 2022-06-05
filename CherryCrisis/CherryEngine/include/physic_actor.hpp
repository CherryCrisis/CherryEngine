#pragma once

#include <cherry_macros.hpp>

#include "behaviour.hpp"
#include "bool3.hpp"

class CharacterController;
class Collider;
class Rigidbody;
class Transform;

namespace PhysicSystem
{
	struct RaycastHit;
	enum class EForceMode;
}

namespace physx
{
	class PxGeometry;
	class PxRigidActor;
	class PxShape;
}


namespace PhysicSystem
{
	class PhysicActor
	{
	protected:
		bool	m_isDynamic			= false;
		bool	m_isEnabled			= true;
		bool	m_isStatic			= false;

		CCMaths::Vector3 m_velocity			= CCMaths::Vector3::Zero;
		CCMaths::Vector3 m_angularVelocity	= CCMaths::Vector3::Zero;
		CCMaths::Vector3 m_InertiaTensor	= CCMaths::Vector3::Zero;
		CCMaths::Vector3 m_oldPos;
		CCMaths::Quaternion m_oldRot;
		
		std::vector<Collider*> m_colliders;

		CharacterController*	m_controller = nullptr;
		physx::PxRigidActor*	m_pxActor = nullptr;
		Rigidbody*				m_rigidbody = nullptr;
		Transform*				m_transform = nullptr;

	public:
		Entity* m_owner = nullptr;

		~PhysicActor();
		
		void Init();					// Set transform pointer
		void InvalidateTransform();		// Set pointer on transform to nullptr if transform is deleted to avoid crash

		void Update();									// Set object transform according to physx updated transform
		void SetActorPosition(Transform* transform);	// Set physx position if the object is moved by any other mean
		void SetActorRotation(Transform* transform);	// Set physx rotation if the object is moved by any other mean
		void SetActorScale(Transform* transform);		// Set physx scale if the object is moved by any other mean
		
		// Create physx's PxActor corresponding to this PhysicActor (dynamic or static, call create for PxShape), and add it to a PxScene
		void CreatePxActor();
		// Remove the PxActor from the PxScene, release (destroy) the PxActor
		void DestroyPxActor();

		// Create a PxShape with the given PxGeometry
		physx::PxShape* CreateShape(const physx::PxGeometry& geometry);
		// Remove the given PxShape from the PxActor
		void			RemoveShape(physx::PxShape* shape);

		
		void AddRigidbody(Rigidbody* rigidbody);					// Link the Rigidbody behaviour to the PhysicActor, reset PxActor if it exists
		void AddCollider(Collider* collider);						// Same as above with a Collider component of any type (Box, Sphere, Capsule)
		void AddController(CharacterController* controller);		// Same as above with a character controller

		void RemoveRigidbody(Rigidbody* rigidbody);					// Unlink the Rigidbody behaviour from the PhysicActor, reset PxActor if it exists
		void RemoveCollider(Collider* collider);					// Same as above with a Collider component of any type (Box, Sphere, Capsule)
		void RemoveController(CharacterController* controller);		// Same as above with a character controller
		void Empty();												// Remove all physic related behaviour from PhysicActor

		// Wrap of physx's addForce and addTorque functions, calls them
		void AddForce(const CCMaths::Vector3& force, EForceMode mode);												
		void AddTorque(const CCMaths::Vector3& force, EForceMode mode);

		// Wrap of physx's Raycast, calls it and ignore result that are itself
		RaycastHit Raycast(const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);

		physx::PxRigidActor* Get() { return m_pxActor; }

		// Call character controller SetRaycastDir if it exists
		void	SetRaycastDir(const CCMaths::Vector3& dir);

		// Set PxActor settings (SetPxActor calls all the following)
		void	SetPxActor();
		void	SetActorConstraints();
		void	SetActorEnabled();
		void	SetActorKinematic();
		void	SetActorGravity();
		void	SetActorDensity();
		void	SetActorMaxVelocities();

		void	SetEnabled(const bool& isEnabled) { m_isEnabled = isEnabled; SetActorEnabled(); }
		bool	GetEnabled() { return m_isEnabled; }

		Rigidbody*				GetRigidbody()	{ return m_rigidbody; }
		CharacterController*	GetController() { return m_controller; }
		std::vector<Collider*>& GetColliders()	{ return m_colliders; }
		bool HasRigidbody();
		bool HasController();
		bool HasColliders();
	};
}