#include <pch.hpp>

#include "cell.hpp"

#include <PxPhysicsAPI.h>

#include "debug.hpp"
#include "physic_manager.hpp"

#include "entity.hpp"
#include "portal.hpp"
#include "renderer.hpp"
#include "sky_renderer.hpp"
#include "viewer.hpp"


Cell::Cell(CCUUID id)
	: Object(id)
{
	m_debug = Debug::GetInstance();
}

Cell::~Cell()
{
	PhysicSystem::PhysicManager::Unregister(this);

	if (m_skyRenderer)
	{
		delete m_skyRenderer;
		m_skyRenderer = nullptr;
	}
}

void Cell::Initialize()
{
	m_skyRenderer = new SkyRenderer(this);

	PhysicSystem::PhysicManager::Register(this);
}

void Cell::AddEntity(Entity* newEntity)
{
	int index = PossessEntity(newEntity);

	if (index != -1)
		return;

	m_entities.push_back(newEntity);
	newEntity->m_cell = this;

	AddEntityToPhysicScene(newEntity);
}

void Cell::AddEntityToPhysicScene(Entity* newEntity)
{
	if (!m_physicCell)
		return;

	PhysicSystem::PhysicActor* actor = PhysicSystem::PhysicManager::FindActor(*newEntity);

	if (actor)
		m_physicCell->AddActor(actor);
}

void Cell::AddRenderer(ARenderer* renderer)
{
	if (!renderer)
		return;

	m_renderers.insert(renderer);

	Viewer* rendererAsViewer = dynamic_cast<Viewer*>(renderer);

	for (Viewer* viewer : m_viewers)
	{
		if (auto pipeline = viewer->m_pipeline.get();
			pipeline && viewer != rendererAsViewer)
			renderer->SubscribeToPipeline(pipeline);
	}
}

void Cell::AddViewer(Viewer* viewer)
{
	if (!viewer)
		return;

	viewer->m_ownerCell = this;

	m_viewers.insert(viewer);

	auto pipeline = viewer->m_pipeline.get();

	if (!pipeline)
		return;

	ARenderer* viewerAsRenderer = dynamic_cast<ARenderer*>(viewer);

	for (ARenderer* renderer : m_renderers)
	{
		if (renderer != viewerAsRenderer)
			renderer->SubscribeToPipeline(pipeline);
	}
}

void Cell::RemoveRenderer(ARenderer* renderer)
{
	if (!renderer)
		return;

	m_renderers.erase(renderer);

	for (auto& viewer : m_viewers)
	{
		if (auto pipeline = viewer->m_pipeline.get())
			renderer->UnsubscribeToPipeline(pipeline);
	}
}

void Cell::RemoveViewer(Viewer* viewer)
{
	if (!viewer)
		return;

	if (viewer->m_ownerCell == this)
		viewer->m_ownerCell = nullptr;

	m_viewers.erase(viewer);

	auto pipeline = viewer->m_pipeline.get();

	if (!pipeline)
		return;

	for (ARenderer* renderer : m_renderers)
		renderer->UnsubscribeToPipeline(pipeline);
}

void Cell::RemoveEntity(Entity* newEntity)
{
	int index = PossessEntity(newEntity);

	if (index == -1)
		return;

	RemoveEntityFromPhysicScene(newEntity);
	newEntity->m_cell = nullptr;
	m_entities[index] = m_entities.back();
	m_entities.pop_back();
}

void Cell::RemoveEntityFromPhysicScene(Entity* newEntity)
{
	if (!m_physicCell)
		return;

	PhysicSystem::PhysicActor* actor = PhysicSystem::PhysicManager::FindActor(*newEntity);

	if (actor)
		m_physicCell->RemoveActor(actor);
}

int Cell::PossessEntity(Entity* entity)
{
	int index = 0;
	for (auto& current : m_entities)
	{
		if (entity == current)
			return index;
		index++;
	}
	return -1;
}

void Cell::LinkPortal(Portal* toLink)
{
	m_portals.push_back(toLink);

	m_portals.back()->m_ownerCell = this;
}

void Cell::LinkPortals(Portal* portal1, Portal* portal2)
{
	portal1->Link(portal2);
}