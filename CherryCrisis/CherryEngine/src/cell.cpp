#include "pch.hpp"

#include "cell.hpp"

#include <PxPhysicsAPI.h>

#include "debug.hpp"
#include "physic_manager.hpp"

#include "portal.hpp"
#include "entity.hpp"
#include "viewer.hpp"
#include "renderer.hpp"
#include "sky_renderer.hpp"

Cell::Cell()
{
	m_debug = Debug::GetInstance();
	m_pxManager = PhysicSystem::PhysicManager::GetInstance();

	m_pxManager->Register(this);
}

Cell::~Cell()
{
	m_pxManager->Unregister(this);
}

void Cell::Initialize()
{
	m_skyRenderer = std::make_unique<SkyRenderer>(this);
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

	PhysicSystem::PhysicActor* actor = m_pxManager->FindActor(*newEntity);

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

	for (Viewer* viewer : m_viewers)
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
	if (!m_physicCell)
		return;

	PhysicSystem::PhysicActor* actor = m_pxManager->FindActor(*newEntity);

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

void Cell::MoveCharacter(float deltaTime)
{
	m_physicCell->MoveCharacterController(deltaTime);
}

void Cell::SetControllerDesc(physx::PxCapsuleControllerDesc& desc)
{
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