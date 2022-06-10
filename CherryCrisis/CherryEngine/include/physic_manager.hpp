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
		bool m_isInDebugMode = false;

		std::vector<PhysicScene*> m_scenes;
		std::vector<PhysicActor*> m_physicActors;

		physx::PxFoundation*	m_foundation = nullptr;
		physx::PxPhysics*		m_physics = nullptr;
		physx::PxPvd*			m_pvd = nullptr;

	public:
		PhysicManager();
		~PhysicManager();

		void CreatePhysX();		// Setup physx
		void DestroyPhysX();	// physx destruction

		physx::PxMaterial* CreateMaterial(float sf = 0.5f, float df = 0.5f, float cr = 0.5f);

		static void Register(Rigidbody* rigidbody);											// Register the Rigidbody behaviour in the PhysicActor of its entity, create the PhysicActor if necessary
		static void Register(Collider* collider);											// Same as above with a Collider component of any type (Box, Sphere, Capsule)
		static void Register(CharacterController* controller);								// Same as above with a character controller

		static void Unregister(Rigidbody* rigidbody, bool checkEmpty = true);				// Unregister the Rigidbody behaviour in the PhysicActor of its entity, destroy the PhysicActor if empty
		static void Unregister(Collider* collider, bool checkEmpty = true);					// Same as above with a Collider component of any type (Box, Sphere, Capsule)
		static void Unregister(CharacterController* controller, bool checkEmpty = true);	// Same as above with a character controller

		static PhysicActor* FindActor(Entity& owningEntity);								// Try and get the PhysicActor of the Entity, return nullptr if it doesn't find it
		static PhysicActor* FindOrCreateActor(Entity& owningEntity);						// Same as above but create the PhysicActor if it doesn't find it
		bool IsActorEmpty(PhysicActor& actor);												// Check if PhysicActor is empty, destroy it in this case

		static void Register(Cell* cell);													// Create a PhysicScene for the Cell and link them
		static void Unregister(Cell* cell);													// Destroy the PhysicScene of the Cell
		
		static PhysicScene* FindScene(Cell* cell);											// Try and get the PhysicScene of the Cell, return nullptr if it doesn't find it
		static PhysicScene* FindOrCreateScene(Cell* cell);									// Same as above but create the PhysicScene if it doesn't find it

		// Instantiate physx for registered PhysicScenes and PhysicActors (create PxScene and PxActor)
		static void Launch();

		// Destroy physx for registered PhysicScenes and PhysicActors (release PxScene and PxActor)
		static void Stop();

		// Call updates of registered PhysicScenes on engine FixedUpdate
		static void Simulate();

		static RaycastHit Raycast(Cell& scene, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);			// Calls following function
		static RaycastHit Raycast(PhysicScene& scene, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);	// Calls RaycastBuff for the PxScene of the PhysicScene

		/* Convert physx PxRaycastHit to the Engine Raycast 
		*	with the PhysicActor instead of the PxActor
		*	and the Collider instead of the pxShape
		*/
		static RaycastHit RaycastFromPxRaycast(const physx::PxRaycastHit& pxHit);
		static physx::PxRaycastBuffer RaycastBuff(PhysicScene& scene, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);	// Calls physx Raycast with a buffer of 16 results

		static void AddForce(Entity* entity, const CCMaths::Vector3& force, EForceMode mode);		// Calls the following function
		static void AddForce(PhysicActor* actor, const CCMaths::Vector3& force, EForceMode mode);	// Calls physx's addForce for the PxActor of the PhysicActor

		// Unlink the PhysicActor and its PxActor from the first PhysicScene and itsPxScene
		// Link them to the second PhysicScene and its PxScene
		static void MoveObjectFromScnToScn(PhysicScene* from, PhysicScene* to, PhysicActor* actor);

		static bool					IsPlaying() { return currentInstance->m_isPlaying; }	// True if physx simulates, else false
		static physx::PxMaterial*	GetMaterial(const uint32_t& index = 0);
		static physx::PxPhysics*	Get() { return currentInstance->m_physics; }
	
		static bool GetIsDebug() { return currentInstance->m_isInDebugMode; }
	};
}