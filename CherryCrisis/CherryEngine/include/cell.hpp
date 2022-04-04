#pragma once

#include "cherry_macros.hpp"

#include "physic_character_controller.hpp"

class Entity;

namespace physx
{
	class PxCapsuleControllerDesc;
}

namespace PhysicSystem
{
	class PhysicScene;
}

class CCENGINE_API Cell
{
private:
	std::vector<Entity*>	m_entities;
	std::vector<Cell*>		m_surroundingCells;
	
	PhysicSystem::PhysicCharacterController m_playerController;

public:
	PhysicSystem::PhysicScene* m_physicCell = nullptr;

	Cell();
	~Cell();

	void AddEntity(Entity* newEntity);
	void AddEntityToPhysicScene(Entity* newEntity);
	void RemoveEntity(Entity* newEntity);

	void LinkToCell(Cell& other);

	void SetControllerDesc(physx::PxCapsuleControllerDesc& desc);
};