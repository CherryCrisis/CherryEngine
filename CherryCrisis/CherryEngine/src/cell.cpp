#include "pch.hpp"

#include "cell.hpp"

#include <PxPhysicsAPI.h>

#include "debug.hpp"
#include "physic_manager.hpp"

#include "entity.hpp"
#include "viewer.hpp"
#include "skybox.hpp"
#include "renderer.hpp"

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
	m_skybox = new Skybox(this);
	m_skybox->Load();
}

void Cell::AddEntity(Entity* newEntity)
{
	if (!this)
		return;

	int index = PossessEntity(newEntity);

	if (index != -1)
		return;

	m_entities.push_back(newEntity);
	newEntity->m_cell = this;

	AddEntityToPhysicScene(newEntity);
}

void Cell::AddEntityToPhysicScene(Entity* newEntity)
{
	if (!this)
		return;

	if (!m_physicCell)
		return;

	PhysicSystem::PhysicActor* actor = m_pxManager->FindActor(*newEntity);

	if (actor)
		m_physicCell->AddActor(actor);
}

void Cell::AddRenderer(ARenderer* renderer)
{
	m_renderers.insert(renderer);

	for (Viewer* viewer : m_viewers)
	{
		if (auto pipeline = viewer->m_pipeline.get())
			renderer->SubscribeToPipeline(pipeline);
	}
}

void Cell::AddViewer(Viewer* viewer)
{
	m_viewers.insert(viewer);

	auto pipeline = viewer->m_pipeline.get();

	if (!pipeline)
		return;

	for (ARenderer* renderer : m_renderers)
		renderer->SubscribeToPipeline(pipeline);
}

void Cell::RemoveRenderer(ARenderer* renderer)
{
	m_renderers.erase(renderer);

	for (Viewer* viewer : m_viewers)
	{
		if (auto pipeline = viewer->m_pipeline.get())
			renderer->UnsubscribeToPipeline(pipeline);
	}
}

void Cell::RemoveViewer(Viewer* viewer)
{
	m_viewers.erase(viewer);

	auto pipeline = viewer->m_pipeline.get();

	if (!pipeline)
		return;

	for (ARenderer* renderer : m_renderers)
		renderer->UnsubscribeToPipeline(pipeline);
}

void Cell::RemoveEntity(Entity* newEntity)
{
	if (!this)
		return;

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

void Cell::AddPortal()
{
	m_portals.push_back(Portal());

	m_portals.back().m_ownerCell = this;
}

void Cell::LinkPortals(Portal portal1, Portal portal2)
{
	portal1.m_linkedPortal = &portal2;
	portal2.m_linkedPortal = &portal1;
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