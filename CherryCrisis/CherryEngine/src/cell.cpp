#include "pch.hpp"

#include "cell.hpp"

#include <PxPhysicsAPI.h>

#include "physic_manager.hpp"
#include "entity.hpp"

Cell::Cell()
{
	PhysicSystem::PhysicManager* pxManager = PhysicSystem::PhysicManager::GetInstance();

	pxManager->Register(this);
}

Cell::~Cell()
{
	PhysicSystem::PhysicManager* pxManager = PhysicSystem::PhysicManager::GetInstance();

	pxManager->Unregister(this);
}

void Cell::AddEntity(Entity* newEntity)
{
	m_entities.push_back(newEntity);
	newEntity->m_cell = this;

	AddEntityToPhysicScene(newEntity);
}

void Cell::AddEntityToPhysicScene(Entity* newEntity)
{
	PhysicSystem::PhysicManager* pxManager = PhysicSystem::PhysicManager::GetInstance();
	PhysicSystem::PhysicActor* actor = pxManager->FindActor(*newEntity);

	if (!m_physicCell)
		return;

	if (actor)
		m_physicCell->AddActor(actor);
}

void Cell::RemoveEntity(Entity* newEntity)
{
	for (size_t i = 0; i < m_entities.size(); ++i)
	{
		if (newEntity == m_entities[i])
		{
			RemoveEntityFromPhysicScene(newEntity);
			newEntity->m_cell = nullptr;
			m_entities[i] = m_entities.back();
			m_entities.pop_back();
		}
	}
}

void Cell::RemoveEntityFromPhysicScene(Entity* newEntity)
{
	PhysicSystem::PhysicManager* pxManager = PhysicSystem::PhysicManager::GetInstance();
	PhysicSystem::PhysicActor* actor = pxManager->FindActor(*newEntity);

	if (!m_physicCell)
		return;

	if (actor)
		m_physicCell->RemoveActor(actor);
}

void Cell::MoveCharacter(float deltaTime)
{
	m_physicCell->MoveCharacterController(deltaTime);

	for (auto& cell : m_surroundingCells)
		cell->m_physicCell->MoveCharacterController(deltaTime);
}

void Cell::LinkToCell(Cell& other)
{
	for (size_t i = 0; i < m_surroundingCells.size(); ++i)
	{
		if (&other == m_surroundingCells[i])
		{
			return;
		}
	}

	m_surroundingCells.push_back(&other);
}

void Cell::SetControllerDesc(physx::PxCapsuleControllerDesc& desc)
{
	//PhysicSystem::PhysicManager* physics = PhysicSystem::PhysicManager::GetInstance();

	//CCMaths::Vector3& p = m_playerController.position;
	//desc.position		= physx::PxExtendedVec3(p.x, p.y, p.z);
	//desc.radius			= m_playerController.radius;
	//desc.height			= m_playerController.height;
	//desc.slopeLimit		= m_playerController.slopeLimit;
	//desc.maxJumpHeight	= m_playerController.maxJumpHeight;
	//desc.contactOffset	= m_playerController.contactOffset;
	//desc.stepOffset		= m_playerController.stepOffset;
	//desc.density		= m_playerController.density;

	//desc.climbingMode = physx::PxCapsuleClimbingMode::Enum::eCONSTRAINED;
	//desc.material = physics->GetMaterial();
}