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
}

void PortalTeleporterComponent::Teleport(PortalComponent* destPortal, const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale)
{
	if (std::shared_ptr<Scene> scene = SceneManager::GetInstance()->m_currentScene)
	{
		Entity* entity = destPortal->GetHostPtr();
		scene->MoveEntityFromCellToCell(GetHost().m_cell, entity->m_cell, GetHostPtr());
		m_transform->SetPosition(newPos);
		m_transform->SetRotation(newRot);
		m_transform->SetScale(newScale);
	}
}

void PortalTeleporterComponent::UpdateEntityClone(const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale)
{
	if (m_cloneEntity)
	{
		//TODO: Optimize ! GetBehaviour<Transform>()
		if (Transform* cloneTransform = m_cloneEntity->GetBehaviour<Transform>())
		{
			cloneTransform->SetPosition(newPos);
			cloneTransform->SetRotation(newRot);
			cloneTransform->SetScale(newScale);
		}
	}
}

void PortalTeleporterComponent::EnterPortal(const PortalComponent* linkedPortal, const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale)
{
	if (std::shared_ptr<Scene> scene = SceneManager::GetInstance()->m_currentScene)
	{
		m_isSlice = 1;
		m_cloneEntity = new Entity(std::format("clone_{}", GetHost().GetName()).c_str(), linkedPortal->GetHost().m_cell);

		enum class ERendererType
		{
			MODEL,
			SHAPE,
		};

		MeshRenderer* meshRdr = nullptr;

		ERendererType rendererType = ERendererType::MODEL;
		meshRdr = GetHost().GetBehaviour<ModelRenderer>();

		if (!meshRdr)
		{
			meshRdr = GetHost().GetBehaviour<ShapeRenderer>();
			rendererType = ERendererType::SHAPE;
		}

		if (meshRdr)
		{
			Transform* cloneTransform = m_cloneEntity->AddBehaviour<Transform>();
			UpdateEntityClone(newPos, newRot, newScale);

			MeshRenderer* cloneMeshRdr = nullptr;

			if (rendererType == ERendererType::MODEL)
				cloneMeshRdr = m_cloneEntity->AddBehaviour<ModelRenderer>();
			else
				cloneMeshRdr = m_cloneEntity->AddBehaviour<ShapeRenderer>();

			if (cloneMeshRdr)
			{
				cloneMeshRdr->m_transform = cloneTransform;

				if (rendererType == ERendererType::SHAPE)
				{
					ShapeRenderer* cloneShapeRdr = dynamic_cast<ShapeRenderer*>(cloneMeshRdr);
					ShapeRenderer* shapeRdr = dynamic_cast<ShapeRenderer*>(meshRdr);
					
					assert(cloneShapeRdr != nullptr || shapeRdr != nullptr);

					cloneShapeRdr->SetType(shapeRdr->GetType());
				}
				else
				{
					cloneMeshRdr->SetMesh(meshRdr->m_mesh);
				}

				cloneMeshRdr->SetMaterial(meshRdr->m_material);
			}
		}

		m_cloneEntity->Initialize();
		scene->AddEntity(m_cloneEntity);

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

