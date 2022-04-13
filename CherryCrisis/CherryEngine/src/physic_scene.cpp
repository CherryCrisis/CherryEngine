#include "pch.hpp"

#include "physic_scene.hpp"

#include <PxPhysicsAPI.h>

#include "physic_manager.hpp"
#include "cell.hpp"

namespace PhysicSystem
{
	void PhysicScene::Update(float deltaTime)
	{
		if (m_paused)
			return;

		m_pxScene->simulate(deltaTime);
		m_pxScene->fetchResults(true);

		for (auto& actor : m_actors)
			actor->Update();
	}

	void PhysicScene::CreatePxScene()
	{
		physx::PxPhysics* pxPhysics = PhysicManager::GetInstance()->Get();

		physx::PxSceneDesc sceneDesc(pxPhysics->getTolerancesScale());

		if (!sceneDesc.cpuDispatcher)
		{
			physx::PxDefaultCpuDispatcher* mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
			sceneDesc.cpuDispatcher = mCpuDispatcher;
		}

		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

		m_pxScene = pxPhysics->createScene(sceneDesc);

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

		//physx::PxControllerManager* manager = PxCreateControllerManager(*m_pxScene);

		//physx::PxCapsuleControllerDesc desc;
		//m_cell->SetControllerDesc(desc);
		//m_playerPxController = manager->createController(desc);

		for (auto& actor : m_actors)
			m_pxScene->addActor(*actor->Get());
	}

	void PhysicScene::DestroyPxScene()
	{
		PX_RELEASE(m_pxScene);
	}

	void PhysicScene::AddCell(Cell* cell)
	{
		m_cell = cell;
	}

	void PhysicScene::RemoveCell(Cell* cell)
	{
		cell = nullptr;
	}

	void PhysicScene::AddActor(PhysicActor* actor)
	{
		int index = PossessActor(actor);

		if (index != -1)
			return;

		m_actors.push_back(actor);

		if (m_pxScene)
		{
			m_pxScene->addActor(*m_actors.back()->Get());
		}
	}

	bool PhysicScene::RemoveActor(PhysicActor* actor)
	{
		int index = PossessActor(actor);

		if (index == -1)
			return false;

		if (m_pxScene)
			m_pxScene->removeActor(*m_actors[index]->Get());

		m_actors[index] = m_actors.back();
		m_actors.pop_back();

		return true;
	}

	int PhysicScene::PossessActor(PhysicActor* actor)
	{
		int index = 0;
		for (auto& current : m_actors)
		{
			if (actor == current)
				return index;
			index++;
		}
		return -1;
	}

	void PhysicScene::MoveCharacterController(float deltaTime)
	{
		m_playerPxController->move(physx::PxVec3(0.f, -9.81f, 0.f ), 1.f, deltaTime, physx::PxControllerFilters());
	}
}