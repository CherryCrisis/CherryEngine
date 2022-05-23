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
	Transform* transform = GetHost().GetOrAddBehaviour<Transform>();
	GetHost().m_OnAwake.Unbind(&PortalTeleporterComponent::Initialize, this);

	EntityNode entityNode;
	GenerateEntityNodesFromTransform(&entityNode, transform);
	m_entityNode = std::make_unique<EntityNode>(entityNode);
}

void PortalTeleporterComponent::Teleport(PortalComponent* destPortal, const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale)
{
	if (std::shared_ptr<Scene> scene = SceneManager::GetInstance()->m_currentScene)
	{
		Entity* entity = destPortal->GetHostPtr();

		if (GetHost().m_cell != entity->m_cell)
			scene->MoveEntityFromCellToCell(GetHost().m_cell, entity->m_cell, GetHostPtr());

		m_entityNode->m_transform->SetPosition(newPos);
		m_entityNode->m_transform->SetRotation(newRot);
		m_entityNode->m_transform->SetScale(newScale);
	}
}

void PortalTeleporterComponent::UpdateEntityMatrix(Transform* transform, const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale)
{
	if (transform)
	{
		transform->SetPosition(newPos);
		transform->SetRotation(newRot);
		transform->SetScale(newScale);
	}
}

void PortalTeleporterComponent::GenerateEntityNodesFromTransform(EntityNode* entityNode, Transform* transform)
{
	if (!transform)
		return;

	entityNode->m_transform = transform;
	entityNode->m_meshRenderer = transform->GetHost().GetBehaviour<ModelRenderer>();

	if (!entityNode->m_meshRenderer)
		entityNode->m_meshRenderer = transform->GetHost().GetBehaviour<ShapeRenderer>();

	auto children = transform->GetChildren();
	for (Transform* childTransform : *children)
	{
		EntityNode meshRendererNodeChild;
		GenerateEntityNodesFromTransform(&meshRendererNodeChild, childTransform);
		entityNode->m_meshRendererChildren.push_back(meshRendererNodeChild);
	}
}

void PortalTeleporterComponent::SetSliceParams(EntityNode* meshRendererNode, bool isSlice, const CCMaths::Vector3& sliceCentre, const CCMaths::Vector3& sliceNormal)
{
	if (!meshRendererNode)
		return;

	if (meshRendererNode->m_meshRenderer)
	{
		meshRendererNode->m_meshRenderer->m_isSlice = isSlice;
		meshRendererNode->m_meshRenderer->m_sliceCentre = sliceCentre;
		meshRendererNode->m_meshRenderer->m_sliceNormal = sliceNormal;
	}

	for (EntityNode& meshRendererNodeChild : meshRendererNode->m_meshRendererChildren)
	{
		SetSliceParams(&meshRendererNodeChild, isSlice, sliceCentre, sliceNormal);
	}
}

void PortalTeleporterComponent::CloneEntities(EntityNode* cloneEntityNode, const EntityNode* entityNode, Cell* destCell, Scene* scene)
{
	if (!entityNode->m_transform)
		return;

	Entity* cloneEntity = new Entity(std::format("Clone_{}", entityNode->m_transform->GetHost().GetName()).c_str(), destCell);
	cloneEntityNode->m_transform = cloneEntity->AddBehaviour<Transform>();

	if (entityNode->m_meshRenderer)
	{
		MeshRenderer* meshRenderer = entityNode->m_meshRenderer;

		if (meshRenderer->m_rendererType == ERendererType::MODEL)
			cloneEntityNode->m_meshRenderer = cloneEntity->AddBehaviour<ModelRenderer>();
		else
			cloneEntityNode->m_meshRenderer = cloneEntity->AddBehaviour<ShapeRenderer>();

		if (cloneEntityNode->m_meshRenderer)
		{
			cloneEntityNode->m_meshRenderer->m_transform = cloneEntityNode->m_transform;

			if (meshRenderer->m_rendererType == ERendererType::SHAPE)
			{
				ShapeRenderer* cloneShapeRdr = dynamic_cast<ShapeRenderer*>(cloneEntityNode->m_meshRenderer);
				ShapeRenderer* shapeRdr = dynamic_cast<ShapeRenderer*>(meshRenderer);

				assert(cloneShapeRdr != nullptr || shapeRdr != nullptr);

				cloneShapeRdr->SetType(shapeRdr->GetType());
			}
			else
			{
				cloneEntityNode->m_meshRenderer->SetMesh(meshRenderer->m_mesh);
			}

			cloneEntityNode->m_meshRenderer->SetMaterial(meshRenderer->m_material);
		}

		cloneEntityNode->m_meshRenderer->m_isSlice = true;
	}

	for (const EntityNode& meshRendererNodeChild : entityNode->m_meshRendererChildren)
	{
		EntityNode cloneMeshRendererNodeChild;
		CloneEntities(&cloneMeshRendererNodeChild, &meshRendererNodeChild, destCell, scene);
		cloneMeshRendererNodeChild.m_transform->SetParent(cloneEntityNode->m_transform);

		cloneEntityNode->m_meshRendererChildren.push_back(cloneMeshRendererNodeChild);

	}
}

void Test(PortalTeleporterComponent::EntityNode* cloneEntityNode, Scene* scene)
{
	cloneEntityNode->m_transform->GetHost().Initialize();
	scene->AddEntity(cloneEntityNode->m_transform->GetHostPtr());

	for (PortalTeleporterComponent::EntityNode& entityNodeChild : cloneEntityNode->m_meshRendererChildren)
		Test(&entityNodeChild, scene);
}


void PortalTeleporterComponent::EnterPortal(const PortalComponent* linkedPortal, const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale)
{
	if (std::shared_ptr<Scene> scene = SceneManager::GetInstance()->m_currentScene)
	{
		m_cloneEntityNode = std::make_unique<EntityNode>();
		CloneEntities(m_cloneEntityNode.get(), m_entityNode.get(), linkedPortal->GetHost().m_cell, scene.get());
		Test(m_cloneEntityNode.get(), scene.get());
	}
}

void PortalTeleporterComponent::ExitPortal()
{
	if (m_cloneEntityNode)
	{
		if (m_cloneEntityNode->m_transform)
		{
			if (std::shared_ptr<Scene> scene = SceneManager::GetInstance()->m_currentScene)
			{
				m_cloneEntityNode->m_transform->GetHost().m_OnUnselected.Invoke();
				scene->RemoveEntity(m_cloneEntityNode->m_transform->GetHostPtr());
				m_cloneEntityNode.reset();
			}
		}
	}

	SetSliceParams(m_entityNode.get(), false, {}, {});
}

