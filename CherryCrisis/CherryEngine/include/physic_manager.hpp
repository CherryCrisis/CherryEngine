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

		void Register(Rigidbody* rigidbody);
		void Register(Collider* collider);
		void Register(CharacterController* controller);

		void Unregister(Rigidbody* rigidbody);
		void Unregister(Collider* collider);
		void Unregister(CharacterController* controller);

		PhysicActor& FindOrCreateActor(Entity& owningEntity);
		PhysicActor* FindActor(Entity& owningEntity);
		bool IsActorEmpty(PhysicActor& actor);

		void Register(Cell* cell);
		void Unregister(Cell* cell);
		
		PhysicScene& FindOrCreateScene(Cell* cell);
		PhysicScene* FindScene(Cell* cell);
		bool IsSceneEmpty(PhysicScene&);

		// Instantiate physx for registered scenes and actors
		void Launch();

		// Destroy physx for registered scenes and actors
		void Stop();

		// Call scenes updates
		void Simulate();

		static RaycastHit Raycast(Cell& scene, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);
		static RaycastHit Raycast(PhysicScene& scene, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);
		static RaycastHit RaycastFromPxRaycast(const physx::PxRaycastHit& pxHit);
		static physx::PxRaycastBuffer RaycastBuff(PhysicScene& scene, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);
		static void AddForce(PhysicActor* actor, const CCMaths::Vector3& force, EForceMode mode);
		static void AddForce(Entity* entity, const CCMaths::Vector3& force, EForceMode mode);

		void MoveObjectFromScnToScn(PhysicScene* from, PhysicScene* to, PhysicActor* actor);

		physx::PxMaterial* GetMaterial(const uint32_t& index = 0);
		physx::PxPhysics* Get() { return m_physics; }
	};
}