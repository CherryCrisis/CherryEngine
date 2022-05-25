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
		int		m_isRemoveLocked	= 0;

		CCMaths::Vector3 m_velocity			= CCMaths::Vector3::Zero;
		CCMaths::Vector3 m_angularVelocity	= CCMaths::Vector3::Zero;
		CCMaths::Vector3 m_InertiaTensor	= CCMaths::Vector3::Zero;
		CCMaths::Vector3 m_oldPos;
		CCMaths::Vector3 m_oldRot;
		
		std::vector<Collider*> m_colliders;

		CharacterController*	m_controller = nullptr;
		physx::PxRigidActor*	m_pxActor = nullptr;
		Rigidbody*				m_rigidbody = nullptr;
		Transform*				m_transform = nullptr;

	public:
		Entity* m_owner = nullptr;

		~PhysicActor();

		void Init();
		void InvalidateTransform();

		void Update();
		void SetActorPosition(const CCMaths::Vector3& position);
		void SetActorRotation(const CCMaths::Vector3& rotation);
		void SetActorScale(const CCMaths::Vector3& scale);
		
		void CreatePxActor();
		void DestroyPxActor();
		void UnlockDelete();

		physx::PxShape* CreateShape(const physx::PxGeometry& geometry);
		void			RemoveShape(physx::PxShape* shape);

		void AddRigidbody(Rigidbody* rigidbody, bool isPlaying = false);
		void AddCollider(Collider* collider, bool isPlaying = false);
		void AddController(CharacterController* controller, bool isPlaying = false);

		void RemoveRigidbody(Rigidbody* rigidbody);
		void RemoveCollider(Collider* collider);
		void RemoveController(CharacterController* controller);
		void Empty();

		void AddForce(const CCMaths::Vector3& force, EForceMode mode);
		void AddTorque(const CCMaths::Vector3& force, EForceMode mode);
		RaycastHit Raycast(const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);

		physx::PxRigidActor* Get() { return m_pxActor; }

		void	SetPxActor();
		void	SetActorConstraints();
		void	SetActorEnabled();
		void	SetActorKinematic();
		void	SetActorGravity();
		void	SetActorDensity();
		void	SetActorMaxVelocities();

		void	SetEnabled(const bool& isEnabled) { m_isEnabled = isEnabled; SetActorEnabled(); }
		bool	GetEnabled() { return m_isEnabled; }

		Rigidbody* GetRigidbody() { return m_rigidbody; }
		bool HasRigidbody();
		bool HasColliders();
	};
}