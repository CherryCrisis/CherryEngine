#pragma once

#include "cherry_macros.hpp"

#include <unordered_set>

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
	std::unordered_set<class Viewer*> m_viewers;
	std::unordered_set<class ARenderer*> m_renderers;
	std::vector<Portal> m_portals;

	class Skybox* m_skybox = nullptr;

public:
	PhysicSystem::PhysicScene* m_physicCell = nullptr;

	Cell();
	~Cell();

	void Initialize();

	void	AddEntity(Entity* newEntity);
	void	AddEntityToPhysicScene(Entity* newEntity);
	void	AddRenderer(ARenderer* renderer);
	void	AddViewer(Viewer* viewer);
	void	RemoveEntity(Entity* newEntity);
	void	RemoveEntityFromPhysicScene(Entity* newEntity);
	void	RemoveRenderer(ARenderer* renderer);
	void	RemoveViewer(Viewer* viewer);
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