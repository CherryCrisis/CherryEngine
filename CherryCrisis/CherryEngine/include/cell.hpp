#pragma once

#include <unordered_set>

#include <cherry_macros.hpp>

#include "object.hpp"

class	Debug;
class	Entity;
struct	Portal;

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

	std::vector<Entity*>					m_entities;
	std::vector<Portal*>					m_portals;
	std::unordered_set<class ARenderer*>	m_renderers;
	std::unordered_set<class Viewer*>		m_viewers;

	Debug*							m_debug = nullptr;
	PhysicSystem::PhysicManager*	m_pxManager = nullptr;

public:
	class SkyRenderer*			m_skyRenderer = nullptr;
	PhysicSystem::PhysicScene*	m_physicCell = nullptr;

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

	void	LinkPortal(Portal* toLink);
	void	LinkPortals(Portal* portal1, Portal* portal2);

	const std::vector<Entity*>& GetEntities() { return m_entities; }

	void SetName(const std::string& name) { m_name = name; }
	const std::string& GetName() { return m_name; }
};