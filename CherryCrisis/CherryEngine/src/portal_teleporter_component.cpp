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
#include "rigidbody.hpp"
#include "physic_actor.hpp"

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
	RemoveCloneEntities();

	GetHost().m_OnStart.Unbind(&PortalTeleporterComponent::Start, this);
}

void PortalTeleporterComponent::RemoveCloneEntities()
{
	if (m_cloneEntityNode)
	{
		if (Entity* entity = m_cloneEntityNode->m_transform->GetHostPtr())
		{
			entity->m_OnDestroyed.Unbind(&PortalTeleporterComponent::OnRemovedClonedEntities, this);
			OnRemovedClonedEntities();
			entity->Destroy();
		}
	}
}


void PortalTeleporterComponent::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetField<Behaviour*>("Transform", m_transform, true, "Transform");
	// Add property
	//m_metadatas.SetProperty("linkedPortalProp", &m_LinkedPortalProp, "PortalTeleporterComponent");
}

void PortalTeleporterComponent::BindToSignals()
{
	GetHost().m_OnAwake.Bind(&PortalTeleporterComponent::Initialize, this);
}

void PortalTeleporterComponent::Initialize()
{
	GetHost().m_OnAwake.Unbind(&PortalTeleporterComponent::Initialize, this);
	GetHost().m_OnStart.Bind(&PortalTeleporterComponent::Start, this);

	m_transform = GetHost().GetBehaviour<Transform>();
}

void PortalTeleporterComponent::ReloadEntitiesClone()
{
	m_entityNode.reset();
	RemoveCloneEntities();

	Transform* transform = GetHost().GetBehaviour<Transform>();

	EntityNode entityNode;
	GenerateEntityNodesFromTransform(&entityNode, transform);
	m_entityNode = std::make_unique<EntityNode>(entityNode);

	if (std::shared_ptr<Scene> scene = SceneManager::GetInstance()->m_currentScene)
	{
		m_cloneEntityNode = std::make_unique<EntityNode>();
		CloneEntities(m_cloneEntityNode.get(), m_entityNode.get(), GetHost().m_cell, scene.get());
		SetIsVisibleEntityNode(m_cloneEntityNode.get(), false);

		m_cloneEntityNode->m_transform->GetHostPtr()->m_OnDestroyed.Bind(&PortalTeleporterComponent::OnRemovedClonedEntities, this);
	}
}


void PortalTeleporterComponent::Start()
{
	GetHost().m_OnStart.Unbind(&PortalTeleporterComponent::Start, this);

	ReloadEntitiesClone();
}

void PortalTeleporterComponent::OnRemovedClonedEntities()
{
	m_cloneEntityNode.reset();
}

void PortalTeleporterComponent::Teleport(PortalComponent* destPortal, const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale)
{
	if (std::shared_ptr<Scene> scene = SceneManager::GetInstance()->m_currentScene)
	{
		Entity* entity = destPortal->GetHostPtr();

		Rigidbody* rb = GetHostPtr()->GetBehaviour<Rigidbody>();

		// Remove and save velocity
		if (rb)
			rb->SaveVelocity();

		if (GetHost().m_cell != entity->m_cell)
			scene->MoveEntityFromCellToCell(GetHost().m_cell, entity->m_cell, GetHostPtr());

		m_entityNode->m_transform->SetGlobalPosition(newPos);
		m_entityNode->m_transform->SetGlobalRotation(newRot);

		// Reapply velocity
		if (rb)
			rb->ReapplyVelocity();
	}
}

void PortalTeleporterComponent::UpdateEntityMatrix(Transform* transform, const Matrix4& newWorldMatrix)
{
	if (transform)
	{
		CCMaths::Vector3 TRS[3] = {};
		Matrix4::Decompose(newWorldMatrix, TRS[0], TRS[1], TRS[2]);

		transform->SetGlobalPosition(TRS[0]);
		transform->SetGlobalRotation(TRS[1]);
		transform->SetGlobalScale(TRS[2]);
	}
}

void PortalTeleporterComponent::UpdateCloneEntitiesMatrix(EntityNode* entityCloneNode, 
	EntityNode* entityNode, const Matrix4& worldMatrixPortal)
{
	UpdateEntityMatrix(entityCloneNode->m_transform, worldMatrixPortal * entityNode->m_transform->GetWorldMatrix());

	int entityChildrenCount = entityNode->m_meshRendererChildren.size();

	for (int id = 0; id < entityChildrenCount; ++id)
	{
		UpdateCloneEntitiesMatrix(&entityCloneNode->m_meshRendererChildren[id], &entityNode->m_meshRendererChildren[id], worldMatrixPortal);
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

	std::unique_ptr<Entity> cloneEntity = std::make_unique<Entity>(std::format("Clone_{}", entityNode->m_transform->GetHost().GetName()).c_str(), destCell);
	cloneEntityNode->m_transform = cloneEntity->AddBehaviour<Transform>();

	cloneEntityNode->m_transform->SetPosition(entityNode->m_transform->GetPosition());
	cloneEntityNode->m_transform->SetRotation(entityNode->m_transform->GetRotation());
	cloneEntityNode->m_transform->SetScale(entityNode->m_transform->GetGlobalScale());

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
	
	cloneEntityNode->m_transform->GetHost().Initialize();
	scene->AddEntity(cloneEntity);
}

void PortalTeleporterComponent::SetIsVisibleEntityNode(EntityNode* entityNode, bool isVisible)
{
	if (entityNode->m_meshRenderer)
		entityNode->m_meshRenderer->m_isVisible = isVisible;

	for (EntityNode& entityNodeChild : entityNode->m_meshRendererChildren)
	{
		SetIsVisibleEntityNode(&entityNodeChild, isVisible);
	}
}


void PortalTeleporterComponent::EnterPortal(const PortalComponent* linkedPortal, const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale)
{
	SetIsVisibleEntityNode(m_cloneEntityNode.get(), true);
	if (Scene* scene = SceneManager::GetInstance()->m_currentScene.get())
	{
		if (linkedPortal->GetHost().m_cell != m_cloneEntityNode->m_transform->GetHost().m_cell)
			scene->MoveEntityFromCellToCell(m_cloneEntityNode->m_transform->GetHost().m_cell, linkedPortal->GetHost().m_cell, m_cloneEntityNode->m_transform->GetHostPtr());
	}
}

void PortalTeleporterComponent::ExitPortal()
{
	if (m_cloneEntityNode)
		SetIsVisibleEntityNode(m_cloneEntityNode.get(), false);
	
	SetSliceParams(m_entityNode.get(), false, {}, {});
}

