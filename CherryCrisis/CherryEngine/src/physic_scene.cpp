#include <pch.hpp>

#include "physic_scene.hpp"

#include "debug.hpp"
#include "physic_manager.hpp"

#include "cell.hpp"
#include "entity.hpp"


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
			pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT |
						physx::PxPairFlag::eNOTIFY_TOUCH_FOUND |
						physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
			return physx::PxFilterFlag::eDEFAULT;
		}
		// generate contacts for all that were not filtered above
		pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT |
					physx::PxPairFlag::eNOTIFY_TOUCH_FOUND |
					physx::PxPairFlag::eNOTIFY_TOUCH_LOST |
					physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;

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

		AddPxActor(actor);
		m_actors.push_back(actor);
	}

	void PhysicScene::AddPxActor(PhysicActor* actor)
	{
		if (m_pxScene)
		{
			m_pxScene->addActor(*actor->Get());
		}
	}

	bool PhysicScene::RemoveActor(PhysicActor* actor)
	{
		int index = PossessActor(actor);

		if (index == -1)
			return false;

		RemovePxActor(m_actors[index]);
		m_actors[index] = m_actors.back();
		m_actors.pop_back();

		return true;
	}

	void PhysicScene::RemovePxActor(PhysicActor* actor)
	{
		if (m_pxScene)
			if (actor->Get())
				m_pxScene->removeActor(*actor->Get());
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

	PhysicActor* PhysicScene::GetActor(int index)
	{
		if (index < 0 || index >= m_actors.size())
			return nullptr;

		else return m_actors[index];
	}

	void PhysicScene::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
	{
		for (physx::PxU32 i = 0; i < nbPairs; i++)
		{
			const physx::PxContactPair& cp = pairs[i];
			
			PhysicActor* actor1 = reinterpret_cast<PhysicActor*>(pairHeader.actors[0]->userData);
			PhysicActor* actor2 = reinterpret_cast<PhysicActor*>(pairHeader.actors[1]->userData);

			if (!actor1 || !actor2)
				return;

			if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
			{
				actor1->m_owner->OnCollisionStay(actor2->m_owner);
				actor2->m_owner->OnCollisionStay(actor1->m_owner);
				continue;
			}

			if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				actor1->m_owner->OnCollisionEnter(actor2->m_owner);
				actor2->m_owner->OnCollisionEnter(actor1->m_owner);
				continue;
			}
			
			if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				actor1->m_owner->OnCollisionExit(actor2->m_owner);
				actor2->m_owner->OnCollisionExit(actor1->m_owner);
			}
		}
	}

	void PhysicScene::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
	{
		for (physx::PxU32 i = 0; i < count; i++)
		{
			const physx::PxTriggerPair& cp = pairs[i];

			PhysicActor* actor1 = reinterpret_cast<PhysicActor*>(pairs[i].triggerActor->userData);
			PhysicActor* actor2 = reinterpret_cast<PhysicActor*>(pairs[i].otherActor->userData);

			if (!actor1 || !actor2)
				return;

			if (cp.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				actor1->m_owner->OnTriggerEnter(actor2->m_owner);
				actor2->m_owner->OnTriggerEnter(actor1->m_owner);
			}

			if (cp.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				actor1->m_owner->OnTriggerExit(actor2->m_owner);
				actor2->m_owner->OnTriggerExit(actor1->m_owner);
			}
		}
	}
}