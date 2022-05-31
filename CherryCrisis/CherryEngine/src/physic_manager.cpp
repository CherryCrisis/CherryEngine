#include <pch.hpp>

#include "physic_manager.hpp"

#include <vector>
#include <PxPhysicsAPI.h>

#include "cell.hpp"
#include "collider.hpp"
#include "character_controller.hpp"
#include "debug.hpp"
#include "entity.hpp"
#include "rigidbody.hpp"
#include "transform.hpp"


PhysicSystem::PhysicManager* Singleton<PhysicSystem::PhysicManager>::currentInstance = nullptr;

using namespace physx;

namespace PhysicSystem
{
	PhysicManager::~PhysicManager()
	{
		if (m_physics)
			Stop();

		for (auto& actor : m_physicActors)
		{
			actor->Empty();
		}

		m_physicActors.clear();

		for (auto& scene : m_scenes)
		{
			delete scene;
			scene = nullptr;
		}

		m_scenes.clear();
	}

	void PhysicManager::CreatePhysX()
	{
		static Debug* debug = Debug::GetInstance();

		static PxDefaultAllocator		allocCallback;
		static PxDefaultErrorCallback	errorCallback;

		if (!m_foundation)
			m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocCallback, errorCallback);

		if (!m_foundation)
		{
			debug->AddLog(ELogType::ERROR, "PxCreateFoundation failed");
			return;
		}

		if (!m_pvd)
		{
			m_pvd = PxCreatePvd(*m_foundation);
			PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
			m_pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
		}

		PxTolerancesScale toleranceScale;
		toleranceScale.length = 100;        // typical length of an object
		toleranceScale.speed = 981;         // typical speed of an object, gravity*1s is a reasonable choice

		if (!m_physics)
			m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, toleranceScale, true, m_pvd);

		if (!m_physics)
			debug->AddLog(ELogType::ERROR, "Could not initialise the PhysX SDK");

		CreateMaterial();
	}

	void PhysicManager::DestroyPhysX()
	{
		PX_RELEASE(m_physics);

		if (m_pvd)
			PX_RELEASE(m_pvd);

		if (m_foundation)
			PX_RELEASE(m_foundation);
	}

	PxMaterial* PhysicManager::CreateMaterial(float sf, float df, float cr)
	{
		return m_physics->createMaterial(sf, df, cr);
	}

	void PhysicManager::Register(Rigidbody* rigidbody)
	{
		if (!currentInstance)
			return;

		PhysicActor* actor = PhysicManager::FindOrCreateActor(rigidbody->GetHost());
		
		rigidbody->m_physicActor = actor;

		actor->AddRigidbody(rigidbody, currentInstance->m_isPlaying);
	}

	void PhysicManager::Register(Collider* collider)
	{
		if (!currentInstance)
			return;

		PhysicActor* actor = PhysicManager::FindOrCreateActor(collider->GetHost());

		collider->m_physicActor = actor;
		actor->AddCollider(collider, currentInstance->m_isPlaying);
	}

	void PhysicManager::Register(CharacterController* controller)
	{
		if (!currentInstance)
			return;

		PhysicActor* actor = PhysicManager::FindOrCreateActor(controller->GetHost());

		controller->m_physicActor = actor;
		actor->AddController(controller, currentInstance->m_isPlaying);
	}

	void PhysicManager::Unregister(Rigidbody* rigidbody)
	{
		if (!currentInstance)
			return;

		PhysicActor* actor = rigidbody->m_physicActor;
		if (!actor)
			return;

		actor->RemoveRigidbody(rigidbody);

		currentInstance->IsActorEmpty(*actor);
	}

	void PhysicManager::Unregister(Collider* collider)
	{
		if (!currentInstance)
			return;

		PhysicActor* actor = collider->m_physicActor;
		if (!actor)
			return;

		actor->RemoveCollider(collider);

		currentInstance->IsActorEmpty(*actor);
	}

	void PhysicManager::Unregister(CharacterController* controller)
	{
		if (!currentInstance)
			return;

		PhysicActor* actor = controller->m_physicActor;
		if (!actor)
			return;

		actor->RemoveController(controller);

		currentInstance->IsActorEmpty(*actor);
	}

	PhysicActor* PhysicManager::FindOrCreateActor(Entity& owningEntity)
	{
		if (!currentInstance)
			return nullptr;

		for (auto& actor : currentInstance->m_physicActors)
		{
			if (actor->m_owner == &owningEntity)
				return actor;
		}

		currentInstance->m_physicActors.push_back(new PhysicActor());
		PhysicActor& newActor = *currentInstance->m_physicActors.back();
		newActor.m_owner = &owningEntity;

		if (currentInstance->m_isPlaying)
			newActor.CreatePxActor();

		newActor.m_owner->m_cell->AddEntityToPhysicScene(newActor.m_owner);

		return &newActor;
	}

	PhysicActor* PhysicManager::FindActor(Entity& owningEntity)
	{
		if (!currentInstance)
			return nullptr;

		for (auto& actor : currentInstance->m_physicActors)
		{
			if (actor->m_owner == &owningEntity)
				return actor;
		}

		return nullptr;
	}

	bool PhysicManager::IsActorEmpty(PhysicActor& actor)
	{
		if (!currentInstance)
			return false;

		if (!actor.HasColliders() && !actor.HasRigidbody() && !actor.HasController())
		{
			for (size_t i = 0; i < m_physicActors.size(); ++i)
			{
				if (&actor == m_physicActors[i])
				{
					delete m_physicActors[i];
					m_physicActors[i] = m_physicActors.back();
					m_physicActors.pop_back();
				}
			}
			return true;
		}

		return false;
	}

	void PhysicManager::Register(Cell* cell)
	{
		if (!currentInstance)
			return;

		PhysicScene* cellScene = PhysicManager::FindOrCreateScene(cell);

		cell->m_physicCell = cellScene;
		cellScene->AddCell(cell);
	}

	void PhysicManager::Unregister(Cell* cell)
	{
		if (!currentInstance)
			return;

		PhysicScene* cellScene = cell->m_physicCell;
		if (!cellScene)
			return;

		cellScene->m_cell = nullptr;
	}

	PhysicScene* PhysicManager::FindOrCreateScene(Cell* cell)
	{
		if (!currentInstance)
			return nullptr;

		for (auto& scene : currentInstance->m_scenes)
		{
			if (scene->m_cell == cell)
				return scene;
		}

		currentInstance->m_scenes.push_back(new PhysicScene());
		PhysicScene& newScene = *currentInstance->m_scenes.back();

		if (currentInstance->m_isPlaying)
			newScene.CreatePxScene();

		return &newScene;
	}

	PhysicScene* PhysicManager::FindScene(Cell* cell)
	{
		if (!currentInstance)
			return nullptr;

		for (auto& scene : currentInstance->m_scenes)
		{
			if (scene->m_cell == cell)
				return scene;
		}

		return nullptr;
	}

	void PhysicManager::Launch()
	{
		if (!currentInstance)
			return;

		currentInstance->m_isPlaying = true;

		currentInstance->CreatePhysX();

		for (auto& scene : currentInstance->m_scenes)
			scene->CreatePxScene();

		for (auto& actor : currentInstance->m_physicActors)
			actor->CreatePxActor();
	}

	void PhysicManager::Stop()
	{
		if (!currentInstance)
			return;

		currentInstance->m_isPlaying = false;

		for (auto& actor : currentInstance->m_physicActors)
			actor->DestroyPxActor();

		for (auto& scene : currentInstance->m_scenes)
			scene->DestroyPxScene();

		currentInstance->DestroyPhysX();
	}

	RaycastHit PhysicManager::Raycast(Cell& cell, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange)
	{
		return PhysicManager::Raycast(*cell.m_physicCell, origin, dir, maxRange);
	}

	RaycastHit PhysicManager::Raycast(PhysicScene& scene, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange)
	{
		physx::PxRaycastBuffer hit = PhysicManager::RaycastBuff(scene, origin, dir, maxRange);

		unsigned int hitNb = (unsigned int)hit.getNbTouches();

		if (hitNb == 0)
			return RaycastHit();

		return PhysicManager::RaycastFromPxRaycast(hit.getTouch(hitNb - 1));
	}

	PxRaycastBuffer PhysicManager::RaycastBuff(PhysicScene& scene, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange)
	{
		static PxRaycastHit hit[16];
		static PxRaycastBuffer hitBuffer(hit, 16);

		bool status = scene.Get()->raycast({ origin.x, origin.y, origin.z }, { dir.x, dir.y, dir.z }, maxRange, hitBuffer);

		return hitBuffer;
	}
	
	RaycastHit PhysicManager::RaycastFromPxRaycast(const physx::PxRaycastHit& pxHit)
	{
		RaycastHit hit;

		if (pxHit.actor)
		{
			hit.actor = reinterpret_cast<PhysicActor*>(pxHit.actor->userData);
			hit.distance = pxHit.distance;
			hit.normal = { pxHit.normal.x, pxHit.normal.y, pxHit.normal.z };
			hit.position = { pxHit.position.x, pxHit.position.y, pxHit.position.z };
		}

		return hit;
	}

	void PhysicManager::AddForce(PhysicActor* actor, const CCMaths::Vector3& force, EForceMode mode)
	{
		actor->AddForce(force, mode);
	}

	void PhysicManager::AddForce(Entity* entity, const CCMaths::Vector3& force, EForceMode mode)
	{
		if (!currentInstance)
			return;

		PhysicActor* actor = currentInstance->FindActor(*entity);
		if (!actor)
			return;

		actor->AddForce(force, mode);
	}

	void PhysicManager::MoveObjectFromScnToScn(PhysicScene* from, PhysicScene* to, PhysicActor* actor)
	{
		if (!from->RemoveActor(actor))
		{
			static Debug* debug = Debug::GetInstance();
			debug->AddLog(ELogType::WARNING, "Actor is not in origin Scene");
		}

		to->AddActor(actor);
	}

	void PhysicManager::Simulate()
	{
		if (!currentInstance)
			return;

		for (auto& scene : currentInstance->m_scenes)
			scene->Update(TimeManager::GetFixedDeltaTime());
	}

	PxMaterial* PhysicManager::GetMaterial(const uint32_t& index)
	{
		if (!currentInstance)
			return 0;

		std::vector<PxMaterial*> materials(currentInstance->m_physics->getNbMaterials());
		if (index < currentInstance->m_physics->getMaterials((PxMaterial**)&materials.front(), (PxU32)materials.size()))
			return materials[index];
		else
			return 0;
	}
}