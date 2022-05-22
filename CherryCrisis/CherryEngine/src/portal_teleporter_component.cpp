#include "pch.hpp"

#include "portal_teleporter_component.hpp"

#include "render_manager.hpp"
#include "scene_manager.hpp"

#include "cell.hpp"
#include "entity.hpp"
#include "transform.hpp"
#include "basic_renderpass.hpp"
#include "skybox_renderpass.hpp"
#include "mixed_rendering_pipeline.hpp"

PortalTeleporterComponent::PortalTeleporterComponent()
{
	PopulateMetadatas();
}

PortalTeleporterComponent::PortalTeleporterComponent(CCUUID& id) : Behaviour(id)
{
	PopulateMetadatas();
}

PortalTeleporterComponent::~PortalTeleporterComponent()
{
	//InvalidateLinkedPortal();
}

void PortalTeleporterComponent::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	// Add property
	//m_metadatas.SetProperty("linkedPortalProp", &m_LinkedPortalProp, "PortalTeleporterComponent");
}

void PortalTeleporterComponent::BindToSignals()
{
	GetHost().m_OnAwake.Bind(&PortalTeleporterComponent::Initialize, this);
}

void PortalTeleporterComponent::Initialize()
{
	m_transform = GetHost().GetOrAddBehaviour<Transform>();
	GetHost().m_OnAwake.Unbind(&PortalTeleporterComponent::Initialize, this);
}

void PortalTeleporterComponent::Teleport(PortalComponent* destPortal, const CCMaths::Vector3& newPos)
{
	if (std::shared_ptr<Scene> scene = SceneManager::GetInstance()->m_currentScene)
	{
		Entity* entity = destPortal->GetHostPtr();
		scene->MoveEntityFromCellToCell(GetHost().m_cell, entity->m_cell, GetHostPtr());
		m_transform->SetPosition(newPos);
	}
}

void PortalTeleporterComponent::EnterPortal()
{

}

void PortalTeleporterComponent::ExitPortal()
{

}

