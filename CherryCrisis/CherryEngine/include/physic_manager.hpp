#pragma once

#include <vector>
#include <string>

#include <cherry_macros.hpp>

#include "singleton.hpp"

#include "physic_actor.hpp"
#include "physic_scene.hpp"

#define PX_RELEASE(x)	if(x)	{ x->release(); x = nullptr; }

class Cell;
class CharacterController;
class Collider;
class Entity;
class Rigidbody;

namespace CCMaths
{
	struct Vector3;
}


namespace PhysicSystem
{
	struct RaycastHit
	{
		CCMaths::Vector3	position	= CCMaths::Vector3::Zero;
		CCMaths::Vector3	normal		= CCMaths::Vector3::Zero;
		float				distance	= 0.0f;

		PhysicActor* actor = nullptr;
		Collider* collider = nullptr;
	};

	enum class EForceMode
	{
		eFORCE,
		eIMPULSE,
		eVELOCITY_CHANGE,
		eACCELERATION
	};

	class CCENGINE_API PhysicManager : public Singleton<PhysicManager>
	{
	private:
		bool m_isPlaying = false;

		std::vector<PhysicScene*> m_scenes;
		std::vector<PhysicActor*> m_physicActors;

		physx::PxFoundation*	m_foundation = nullptr;
		physx::PxPhysics*		m_physics = nullptr;
		physx::PxPvd*			m_pvd = nullptr;

	public:
		~PhysicManager();

		void CreatePhysX();
		void DestroyPhysX();

		physx::PxMaterial* CreateMaterial(float sf = 0.5f, float df = 0.5f, float cr = 0.1f);

		static void Register(Rigidbody* rigidbody);
		static void Register(Collider* collider);
		static void Register(CharacterController* controller);

		static void Unregister(Rigidbody* rigidbody);
		static void Unregister(Collider* collider);
		static void Unregister(CharacterController* controller);

		static PhysicActor* FindActor(Entity& owningEntity);
		static PhysicActor* FindOrCreateActor(Entity& owningEntity);
		bool IsActorEmpty(PhysicActor& actor);

		static void Register(Cell* cell);
		static void Unregister(Cell* cell);
		
		static PhysicScene* FindScene(Cell* cell);
		static PhysicScene* FindOrCreateScene(Cell* cell);

		// Instantiate physx for registered scenes and actors
		static void Launch();

		// Destroy physx for registered scenes and actors
		static void Stop();

		// Call scenes updates
		static void Simulate();

		static RaycastHit Raycast(Cell& scene, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);
		static RaycastHit Raycast(PhysicScene& scene, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);
		static RaycastHit RaycastFromPxRaycast(const physx::PxRaycastHit& pxHit);
		static physx::PxRaycastBuffer RaycastBuff(PhysicScene& scene, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);
		static void AddForce(PhysicActor* actor, const CCMaths::Vector3& force, EForceMode mode);
		static void AddForce(Entity* entity, const CCMaths::Vector3& force, EForceMode mode);

		static void MoveObjectFromScnToScn(PhysicScene* from, PhysicScene* to, PhysicActor* actor);

		static physx::PxMaterial* GetMaterial(const uint32_t& index = 0);
		static physx::PxPhysics* Get() { return currentInstance->m_physics; }
	};
}