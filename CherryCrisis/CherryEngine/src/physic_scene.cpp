#include "pch.hpp"

#include "physic_scene.hpp"


#include "physic_manager.hpp"
#include "cell.hpp"

#include "debug.hpp"

namespace PhysicSystem
{
	void PhysicScene::Update(float deltaTime)
	{
		if (m_paused)
			return;

		m_pxScene->collide(deltaTime);
		m_pxScene->fetchCollision(true);
		m_pxScene->advance();
		m_pxScene->fetchResults(true);

		for (auto& actor : m_actors)
			actor->Update();
	}

	physx::PxFilterFlags FilterShader(
		physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
	{
		// let triggers through
		if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
			return physx::PxFilterFlag::eDEFAULT;
		}
		// generate contacts for all that were not filtered above
		pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

		// trigger the contact callback for pairs (A,B) where 
		// the filtermask of A contains the ID of B and vice versa.
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;

		return physx::PxFilterFlag::eDEFAULT;
	}

	void PhysicScene::CreatePxScene()
	{
		static Debug* debug = Debug::GetInstance();

		physx::PxPhysics* pxPhysics = PhysicManager::GetInstance()->Get();

		physx::PxSceneDesc sceneDesc(pxPhysics->getTolerancesScale());

		if (!sceneDesc.cpuDispatcher)
		{
			physx::PxDefaultCpuDispatcher* mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
			sceneDesc.cpuDispatcher = mCpuDispatcher;
		}

		sceneDesc.filterShader = FilterShader;
		sceneDesc.simulationEventCallback = this;
		//sceneDesc.flags |= physx::PxSceneFlag::eREQUIRE_RW_LOCK;

		m_pxScene = pxPhysics->createScene(sceneDesc);

		if (!m_pxScene)
		{
			debug->AddLog(ELogType::ERROR, "Could not initialise the PhysX scene");
			return;
		}

		m_pxScene->setGravity({ 0.0f, -m_gravity, 0.0f });

		physx::PxPvdSceneClient* pvdClient = m_pxScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}

		for (auto& actor : m_actors)
		{
			if (actor->Get()) 
				m_pxScene->addActor(*actor->Get());
		}
	}

	void PhysicScene::DestroyPxScene()
	{
		PX_RELEASE(m_pxScene);
	}

	void PhysicScene::AddCell(Cell* cell)
	{
		m_cell = cell;
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

	void PhysicScene::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
	{
		for (physx::PxU32 i = 0; i < nbPairs; i++)
		{
			const physx::PxContactPair& cp = pairs[i];

			if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				PhysicActor* actor1 = reinterpret_cast<PhysicActor*>(pairHeader.actors[0]->userData);
				PhysicActor* actor2 = reinterpret_cast<PhysicActor*>(pairHeader.actors[1]->userData);

				actor1->m_owner->OnCollisionEnter(actor2->m_owner);
				actor2->m_owner->OnCollisionEnter(actor1->m_owner);
			}
		}
	}

	void PhysicScene::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
	{
		for (physx::PxU32 i = 0; i < count; i++)
		{
			// ignore pairs when shapes have been deleted
			if (pairs[i].flags & (physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
				continue;

			PhysicActor* actor1 = reinterpret_cast<PhysicActor*>(pairs[i].triggerActor->userData);
			PhysicActor* actor2 = reinterpret_cast<PhysicActor*>(pairs[i].otherActor->userData);

			actor1->m_owner->OnTriggerEnter(actor2->m_owner);
		}
	}


	void PhysicScene::MoveCharacterController(float deltaTime)
	{
		m_playerPxController->move(physx::PxVec3(0.f, -9.81f, 0.f ), 1.f, deltaTime, physx::PxControllerFilters());
	}
}