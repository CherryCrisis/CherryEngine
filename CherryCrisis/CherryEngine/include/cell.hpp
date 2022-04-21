#pragma once

#include "cherry_macros.hpp"

class Entity;
class Cell;
class Debug;

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
	class PhysicManager;
	class PhysicScene;
}

class CCENGINE_API Cell
{
private:
	std::string m_name;

	Debug* m_debug = nullptr;
	PhysicSystem::PhysicManager* m_pxManager = nullptr;

	std::vector<Entity*> m_entities;
	std::vector<Portal> m_portals;

public:
	PhysicSystem::PhysicScene* m_physicCell = nullptr;

	Cell();
	~Cell();

	void	AddEntity(Entity* newEntity);
	void	AddEntityToPhysicScene(Entity* newEntity);
	void	RemoveEntity(Entity* newEntity);
	void	RemoveEntityFromPhysicScene(Entity* newEntity);
	int		PossessEntity(Entity* entity);


	const std::vector<Entity*> GetEntities() { return m_entities; }

	void	AddPortal();
	void	LinkPortals(Portal portal1, Portal portal2);

	// TODO: Add info of movement (direction, speed, ...)
	// TODO: Choose if using pxCharacterController or personal one
	void	MoveCharacter(float deltaTime);
	void	SetControllerDesc(physx::PxCapsuleControllerDesc& desc);

	void SetName(const std::string& name) { m_name = name; }
	const std::string& GetName() { return m_name; }
};