#include "pch.hpp"

#include "physic_scene.hpp"

#include <PxPhysicsAPI.h>

#include "physic_actor.hpp"

namespace PhysicSystem
{
	void PhysicScene::Update(float deltaTime)
	{
		if (m_paused)
			return;

		m_pxScene->simulate(deltaTime);
		m_pxScene->fetchResults(true);
	}

	void PhysicScene::CreatePxScene()
	{
		physx::PxPhysics* physics = PhysicManager::GetInstance()->Get();

		physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());

		if (!sceneDesc.cpuDispatcher)
		{
			physx::PxDefaultCpuDispatcher* mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
			sceneDesc.cpuDispatcher = mCpuDispatcher;
		}

		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

		m_pxScene = physics->createScene(sceneDesc);

		// TODO: change error (throw -> Log)
		if (!m_pxScene)
			throw("PhysicsEngine::Scene::Init, Could not initialise the scene.");

		m_pxScene->setGravity({ 0.0f, -m_gravity, 0.0f });

		physx::PxPvdSceneClient* pvdClient = m_pxScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
	}

	void PhysicScene::DestroyPxScene()
	{
		PX_RELEASE(m_pxScene);
	}

	void PhysicScene::AddActor(PhysicActor* actor)
	{
		actors.push_back(actor);

		m_pxScene->addActor(*actor->Get());
	}

	bool PhysicScene::RemoveActor(PhysicActor* actor)
	{
		int index = PossessActor(actor);

		if (index == -1)
			return false;

		m_pxScene->removeActor(*actors[index]->Get());

		actors[index] = actors.back();
		actors.pop_back();

		return true;
	}

	int PhysicScene::PossessActor(PhysicActor* actor)
	{
		int index = 0;
		for (auto& current : actors)
		{
			if (actor == current)
				return index;
			index++;
		}
		return -1;
	}
}