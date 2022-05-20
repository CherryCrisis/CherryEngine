#pragma once

#include "cherry_macros.hpp"

#include <unordered_set>
#include "object.hpp"

class Entity;
class Debug;
struct Portal;

namespace physx
{
	class PxCapsuleControllerDesc;
}

namespace PhysicSystem
{
	class PhysicManager;
	class PhysicScene;
}

class CCENGINE_API Cell : public Object
{
private:
	std::string m_name;

	Debug* m_debug = nullptr;
	PhysicSystem::PhysicManager* m_pxManager = nullptr;

	std::vector<Entity*> m_entities;
	std::unordered_set<class Viewer*> m_viewers;
	std::unordered_set<class ARenderer*> m_renderers;
	std::vector<Portal*> m_portals;

public:
	class SkyRenderer* m_skyRenderer = nullptr;
	PhysicSystem::PhysicScene* m_physicCell = nullptr;

	Cell(CCUUID id = {});
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

	const std::vector<Entity*>& GetEntities() { return m_entities; }

	void	LinkPortal(Portal* toLink);
	void	LinkPortals(Portal* portal1, Portal* portal2);

	void SetName(const std::string& name) { m_name = name; }
	const std::string& GetName() { return m_name; }
};