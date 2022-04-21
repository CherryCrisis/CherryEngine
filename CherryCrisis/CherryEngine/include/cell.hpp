#pragma once

#include "cherry_macros.hpp"

class Entity;
class Cell;

struct Portal
{
	Portal* m_linkedPortal = nullptr;
	Cell*	m_ownerCell = nullptr;
};

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
	std::vector<Entity*> m_entities;
	std::vector<Portal> m_portals;

public:
	PhysicSystem::PhysicScene* m_physicCell = nullptr;

	Cell();
	~Cell();

	void AddEntity(Entity* newEntity);
	void AddEntityToPhysicScene(Entity* newEntity);
	void RemoveEntity(Entity* newEntity);
	void RemoveEntityFromPhysicScene(Entity* newEntity);

	void AddPortal();

	void LinkPortals(Portal portal1, Portal portal2);

	// TODO: Add info of movement (direction, speed, ...)
	void MoveCharacter(float deltaTime);

	void SetControllerDesc(physx::PxCapsuleControllerDesc& desc);
};