#include "pch.hpp"

#include "physic_manager.hpp"

#include <vector>
#include <PxPhysicsAPI.h>

#include "collider.hpp"
#include "rigidbody.hpp"
#include "transform.hpp"
#include "cell.hpp"

#include "debug.hpp"

PhysicSystem::PhysicManager* Singleton<PhysicSystem::PhysicManager>::currentInstance = nullptr;

namespace PhysicSystem
{
	PhysicManager::PhysicManager()
	{
		static Debug* debug = Debug::GetInstance();

		static physx::PxDefaultAllocator		allocCallback;
		static physx::PxDefaultErrorCallback	errorCallback;

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
			physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
			m_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
		}

		physx::PxTolerancesScale toleranceScale;
		toleranceScale.length = 100;        // typical length of an object
		toleranceScale.speed = 981;         // typical speed of an object, gravity*1s is a reasonable choice

		if (!m_physics)
			m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, toleranceScale, true, m_pvd);

		if (!m_physics)
			debug->AddLog(ELogType::ERROR, "Could not initialise the PhysX SDK");

		CreateMaterial();
	}

	PhysicManager::~PhysicManager()
	{
		for (auto& actor : m_physicActors)
		{
			actor->Empty();
		}

		m_physicActors.clear();

		for (auto& scene : m_scenes)
		{
			physx::PxScene* pxScene = scene->Get();
			PX_RELEASE(pxScene);

			delete scene;
			scene = nullptr;
		}

		m_scenes.clear();

		// TODO: Remove
		delete cell;

		PX_RELEASE(m_physics);

		if (m_pvd)
			PX_RELEASE(m_pvd);

		if (m_foundation)
			PX_RELEASE(m_foundation);
	}

	physx::PxMaterial* PhysicManager::CreateMaterial(float sf, float df, float cr)
	{
		return m_physics->createMaterial(sf, df, cr);
	}

	void PhysicManager::Register(Rigidbody* rigidbody)
	{
		if (!cell)
		cell = new Cell();

		PhysicActor& actor = FindOrCreateActor(rigidbody->GetHost());
		
		rigidbody->m_physicActor = &actor;
		actor.AddRigidbody(rigidbody, m_isPlaying);

		if (!actor.m_owner->m_cell)
			actor.m_owner->m_cell = cell;

		actor.m_owner->m_cell->AddEntity(actor.m_owner);
	}

	void PhysicManager::Register(Collider* collider)
	{
		if (!cell)
			cell = new Cell();

		PhysicActor& actor = FindOrCreateActor(collider->GetHost());

		collider->m_physicActor = &actor;
		actor.AddCollider(collider, m_isPlaying);
		
		if (!actor.m_owner->m_cell)
			actor.m_owner->m_cell = cell;

		actor.m_owner->m_cell->AddEntity(actor.m_owner);
	}

	void PhysicManager::Unregister(Rigidbody* rigidbody)
	{
		PhysicActor* actor = rigidbody->m_physicActor;
		if (!actor)
			return;

		actor->RemoveRigidbody(rigidbody);

		IsActorEmpty(*actor);
	}

	void PhysicManager::Unregister(Collider* collider)
	{
		PhysicActor* actor = collider->m_physicActor;
		if (!actor)
			return;

		actor->RemoveCollider(collider);

		IsActorEmpty(*actor);
	}

	PhysicActor& PhysicManager::FindOrCreateActor(Entity& owningEntity)
	{
		for (auto& actor : m_physicActors)
		{
			if (actor->m_owner == &owningEntity)
				return *actor;
		}

		m_physicActors.push_back(new PhysicActor());
		PhysicActor& newActor = *m_physicActors.back();
		newActor.m_owner = &owningEntity;
		return newActor;
	}

	PhysicActor* PhysicManager::FindActor(Entity& owningEntity)
	{
		for (auto& actor : m_physicActors)
		{
			if (actor->m_owner == &owningEntity)
				return actor;
		}

		return nullptr;
	}

	bool PhysicManager::IsActorEmpty(PhysicActor& actor)
	{
		if (!actor.HasColliders() && !actor.HasRigidbody())
		{
			for (size_t i = 0; i < m_physicActors.size(); ++i)
			{
				if (&actor == m_physicActors[i])
				{
					actor.m_owner->m_cell->RemoveEntity(actor.m_owner);
					actor.DestroyPxActor();
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
		PhysicScene& cellScene = FindOrCreateScene(cell);

		cell->m_physicCell = &cellScene;
		cellScene.AddCell(cell);
	}

	void PhysicManager::Unregister(Cell* cell)
	{
		PhysicScene* cellScene = cell->m_physicCell;
		if (!cellScene)
			return;

		cellScene->RemoveCell(cell);

		IsSceneEmpty(*cellScene);
	}

	PhysicScene& PhysicManager::FindOrCreateScene(Cell* cell)
	{
		for (auto& scene : m_scenes)
		{
			if (scene->m_cell == cell)
				return *scene;
		}

		m_scenes.push_back(new PhysicScene());
		PhysicScene& newScene = *m_scenes.back();
		return newScene;
	}

	PhysicScene* PhysicManager::FindScene(Cell* cell)
	{
		for (auto& scene : m_scenes)
		{
			if (scene->m_cell == cell)
				return scene;
		}

		return nullptr;
	}

	bool PhysicManager::IsSceneEmpty(PhysicScene& scene)
	{
		if (!scene.m_cell)
		{
			for (size_t i = 0; i < m_scenes.size(); ++i)
			{
				if (&scene == m_scenes[i])
				{
					m_scenes[i] = m_scenes.back();
					m_scenes.pop_back();
				}
			}
			return true;
		}

		return false;
	}

	void PhysicManager::Launch()
	{
		m_isPlaying = true;

		for (auto& actor : m_physicActors)
			actor->CreatePxActor();

		for (auto& scene : m_scenes)
			scene->CreatePxScene();
	}

	void PhysicManager::Stop()
	{
		m_isPlaying = false;

		for (auto& actor : m_physicActors)
			actor->DestroyPxActor();

		for (auto& scene : m_scenes)
			scene->DestroyPxScene();
	}

	Raycast PhysicManager::RaycastInScene(PhysicScene& scene, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange)
	{
		Raycast hit;

		bool status = scene.Get()->raycast({ origin.x, origin.y, origin.z }, { dir.x, dir.y, dir.z }, maxRange, hit);

		return hit;
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

	void PhysicManager::Simulate(float deltaTime)
	{
		for (auto& scene : m_scenes)
			scene->Update(deltaTime);
	}

	physx::PxMaterial* PhysicManager::GetMaterial(const uint32_t& index)
	{
		std::vector<physx::PxMaterial*> materials(m_physics->getNbMaterials());
		if (index < m_physics->getMaterials((physx::PxMaterial**)&materials.front(), (physx::PxU32)materials.size()))
			return materials[index];
		else
			return 0;
	}
}