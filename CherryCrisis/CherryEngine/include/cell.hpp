#pragma once

#include "cherry_macros.hpp"

#include <deque>

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
	std::deque<Entity*>	m_entities;
	std::deque<Cell*>	m_surroundingCells;

public:
	PhysicSystem::PhysicScene* m_physicCell = nullptr;

	Cell();
	~Cell();

	void AddEntity(Entity* newEntity);
	void AddEntityToPhysicScene(Entity* newEntity);
	void RemoveEntity(Entity* newEntity);
	void RemoveEntityFromPhysicScene(Entity* newEntity);

	// TODO: Add info of movement (direction, speed, ...)
	void MoveCharacter(float deltaTime);
	
	void LinkToCell(Cell& other);

	void SetControllerDesc(physx::PxCapsuleControllerDesc& desc);
};