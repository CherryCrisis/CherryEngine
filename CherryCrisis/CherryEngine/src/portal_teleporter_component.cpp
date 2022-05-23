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
#include "shape_renderer.hpp"

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

void PortalTeleporterComponent::UpdateEntityClone(const CCMaths::Vector3& newPos)
{
	if (m_cloneEntity)
	{
		//TODO: Optimize ! GetBehaviour<Transform>()
		if (Transform* cloneTransform = m_cloneEntity->GetBehaviour<Transform>())
		{
			cloneTransform->SetPosition(newPos);
			//cloneTransform->SetRotation(m_transform->GetRotation() + CCMaths::PI);
			//cloneTransform->SetScale(m_transform->GetScale());
		}
	}
}

void PortalTeleporterComponent::EnterPortal(const PortalComponent* linkedPortal)
{
	if (std::shared_ptr<Scene> scene = SceneManager::GetInstance()->m_currentScene)
	{
		m_isSlice = 1;
		Entity* cloneEntity = new Entity(std::format("clone_{}", GetHost().GetName()).c_str(), linkedPortal->GetHost().m_cell);

		//TODO: Optimize !
		if (ShapeRenderer* shapeRdr = GetHost().GetBehaviour<ShapeRenderer>())
		{
			Transform* cloneTransform = cloneEntity->AddBehaviour<Transform>();

			cloneTransform->SetPosition(m_transform->GetPosition());
			cloneTransform->SetRotation(m_transform->GetRotation());
			cloneTransform->SetScale(m_transform->GetScale());

			ShapeRenderer* cloneShapeRdr = cloneEntity->AddBehaviour<ShapeRenderer>();
			cloneShapeRdr->m_transform = cloneTransform;
			cloneShapeRdr->SetMesh(shapeRdr->m_mesh);
			cloneShapeRdr->SetMaterial(shapeRdr->m_material);
		}

		cloneEntity->Initialize();
		scene->AddEntity(cloneEntity);

		m_cloneEntity = cloneEntity;
	}
}

void PortalTeleporterComponent::ExitPortal()
{
	if (m_cloneEntity)
	{
		if (std::shared_ptr<Scene> scene = SceneManager::GetInstance()->m_currentScene)
		{
			m_cloneEntity->m_OnUnselected.Invoke();
			scene->RemoveEntity(m_cloneEntity);
			m_cloneEntity = nullptr;
		}
	}

	m_isSlice = 0;
}

