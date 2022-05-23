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
#include "model_renderer.hpp"

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

	if (m_meshRenderer = GetHost().GetBehaviour<ModelRenderer>())
	{
		if (!m_meshRenderer)
		{
			m_meshRenderer = GetHost().GetBehaviour<ShapeRenderer>();
		}
	}
}

void PortalTeleporterComponent::Teleport(PortalComponent* destPortal, const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale)
{
	if (std::shared_ptr<Scene> scene = SceneManager::GetInstance()->m_currentScene)
	{
		Entity* entity = destPortal->GetHostPtr();

		if (GetHost().m_cell != entity->m_cell)
			scene->MoveEntityFromCellToCell(GetHost().m_cell, entity->m_cell, GetHostPtr());

		m_transform->SetPosition(newPos);
		m_transform->SetRotation(newRot);
		m_transform->SetScale(newScale);
	}
}

void PortalTeleporterComponent::UpdateEntityClone(const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale)
{
	if (m_cloneTransform)
	{
		m_cloneTransform->SetPosition(newPos);
		m_cloneTransform->SetRotation(newRot);
		m_cloneTransform->SetScale(newScale);
	}
}

void PortalTeleporterComponent::EnterPortal(const PortalComponent* linkedPortal, const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale)
{
	if (std::shared_ptr<Scene> scene = SceneManager::GetInstance()->m_currentScene)
	{
		m_meshRenderer->m_isSlice = 1;

		Entity* cloneEntity = new Entity(std::format("clone_{}", GetHost().GetName()).c_str(), linkedPortal->GetHost().m_cell);

		if (m_meshRenderer)
		{
			m_cloneTransform = cloneEntity->AddBehaviour<Transform>();
			UpdateEntityClone(newPos, newRot, newScale);

			m_cloneMeshRenderer = nullptr;

			if (m_meshRenderer->m_rendererType == ERendererType::MODEL)
				m_cloneMeshRenderer = cloneEntity->AddBehaviour<ModelRenderer>();
			else
				m_cloneMeshRenderer = cloneEntity->AddBehaviour<ShapeRenderer>();

			if (m_cloneMeshRenderer)
			{
				m_cloneMeshRenderer->m_transform = m_cloneTransform;

				if (m_meshRenderer->m_rendererType == ERendererType::SHAPE)
				{
					ShapeRenderer* cloneShapeRdr = dynamic_cast<ShapeRenderer*>(m_cloneMeshRenderer);
					ShapeRenderer* shapeRdr = dynamic_cast<ShapeRenderer*>(m_meshRenderer);
					
					assert(cloneShapeRdr != nullptr || shapeRdr != nullptr);

					cloneShapeRdr->SetType(shapeRdr->GetType());
				}
				else
				{
					m_cloneMeshRenderer->SetMesh(m_meshRenderer->m_mesh);
				}

				m_cloneMeshRenderer->SetMaterial(m_meshRenderer->m_material);
			}
		}

		m_cloneMeshRenderer->m_isSlice = 1;
		m_cloneTransform->GetHost().Initialize();
		scene->AddEntity(m_cloneTransform->GetHostPtr());
	}
}

void PortalTeleporterComponent::ExitPortal()
{
	if (m_cloneTransform)
	{
		if (std::shared_ptr<Scene> scene = SceneManager::GetInstance()->m_currentScene)
		{
			m_cloneTransform->GetHost().m_OnUnselected.Invoke();
			scene->RemoveEntity(m_cloneTransform->GetHostPtr());
			m_cloneTransform = nullptr;
			m_cloneMeshRenderer = nullptr;
		}
	}

	m_meshRenderer->m_isSlice = 0;
}

