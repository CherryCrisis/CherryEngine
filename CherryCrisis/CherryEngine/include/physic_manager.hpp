#pragma once

#include "cherry_macros.hpp"

#include <vector>
#include <string>

#include "singleton.hpp"

#include "physic_actor.hpp"
#include "physic_scene.hpp"
#include "physic_utils.hpp"

#define PX_RELEASE(x)	if(x)	{ x->release(); x = nullptr; }
#define CCPhysicManager PhysicSystem::PhysicManager

namespace physx
{
	class PxFoundation;
	class PxPvd;
	class PxPhysics;
	class PxMaterial;
	class PxTransform;
}

namespace CCMaths
{
	struct Vector3;
}

class Rigidbody;
class Collider;
class Cell;

namespace PhysicSystem
{
	class CCENGINE_API PhysicManager : public Singleton<PhysicManager>
	{
	private:
		physx::PxFoundation* m_foundation = nullptr;
		physx::PxPvd* m_pvd = nullptr;
		physx::PxPhysics* m_physics = nullptr;

		std::vector<PhysicActor*> m_physicActors;
		std::vector<PhysicScene*> m_scenes;

		bool m_isPlaying = false;

	public:
		PhysicManager();
		~PhysicManager();

		physx::PxMaterial* CreateMaterial(float sf = 0.0f, float df = 0.0f, float cr = 0.0f);

		void Register(Rigidbody* rigidbody);
		void Register(Collider* collider);

		void Unregister(Rigidbody* rigidbody);
		void Unregister(Collider* collider);

		PhysicActor& FindOrCreateActor(Entity& owningEntity);
		PhysicActor* FindActor(Entity& owningEntity);
		bool IsActorEmpty(PhysicActor& actor);
	
		// TODO: Implement follozing functions when Cell system is done

		void Register(Cell* cell);
		void Unregister(Cell* cell);
		
		PhysicScene& FindOrCreateScene(Cell* cell);
		PhysicScene* FindScene(Cell* cell);
		bool IsSceneEmpty(PhysicScene&);  // -> might be replace by just a destroy because a PhysicScene should only have one Cell

		// Instantiate physx for registered scenes and actors
		void Launch();

		// Destroy physx for registered scenes and actors
		void Stop();

		// Call scenes updates
		void Simulate(float deltaTime);

		Raycast RaycastInScene(PhysicScene& scene, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);
		void MoveObjectFromScnToScn(PhysicScene* from, PhysicScene* to, PhysicActor* actor);

		physx::PxMaterial* GetMaterial(const uint32_t& index = 0);
		physx::PxPhysics* Get() { return m_physics; }
	};
}