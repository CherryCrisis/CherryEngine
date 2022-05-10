#include "pch.hpp"

#include "scene.hpp"

#include <iostream>

#include "resource_manager.hpp"
#include "render_manager.hpp"

// TODO: Remove this
#include "transform.hpp"
#include "light_component.hpp"
#include "model_renderer.hpp"
#include "camera_component.hpp"
#include "scripted_behaviour.hpp"

#include "model_base.hpp"
#include "mesh.hpp"

#include "serializer.hpp"

void Scene::Delete()
{
	for (const Entity* entity : m_entities)
		delete entity;
}

Scene::~Scene()
{
	if (GetResourceState() != EResourceState::DESTROYED)
		Delete();
}

void Scene::Initialize()
{
	for (Entity* entity : m_entities)
		entity->Initialize();
}

void Scene::Update()
{
 	for (Entity* entity : m_entities)
		entity->Update();
}

void Scene::PopulateEmpty() 
{
	Entity* light = new Entity("Light");
	light->AddBehaviour<LightComponent>(); 	AddEntity(light);

	Entity* camera = new Entity("Camera");
	camera->AddBehaviour<Transform>();
	auto cameraComp = camera->AddBehaviour<CameraComponent>();
	cameraComp->BindToSignals();

	ScriptedBehaviour* bhave1 = camera->AddBehaviour<ScriptedBehaviour>();
	bhave1->SetScriptClass("CameraController"); bhave1->BindToSignals();
	AddEntity(camera);
}

void Scene::AddEntity(Entity* toAdd)
{
	m_entities.push_back(toAdd);
}

void Scene::RemoveEntity(Entity* toRemove) 
{
	if (!toRemove)
		return;

	Transform* transform;
	toRemove->TryGetBehaviour(transform);
	if (transform && transform->GetChildren().size() > 0 )
	{
		std::vector<Transform*> children = transform->GetChildren();

		for (int i = 0; i < children.size(); i++)
			RemoveEntity(&children[i]->GetHost());
	}

	m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), toRemove), m_entities.end());
	toRemove->Destroy();
}

void Scene::RemoveEntity(const std::string& name)
{
	//TODO: Do this
}

void Scene::Load(std::shared_ptr<Scene> scene)
{
	Serializer::UnserializeScene(scene);
}

bool Scene::Save() 
{
	return Serializer::SerializeScene(this);
}

bool Scene::SaveAs(const char* filepath)
{
	return Serializer::SerializeScene(this, filepath);
}

void Scene::GenerateEntitiesRecursive(ModelNode* node, Entity* parentEntity, std::vector<Entity*>& entities)
{
	Entity* entity = new Entity();


	if (!node->m_nodeName.empty())
		entity->SetName(node->m_nodeName);

	Transform* transform = entity->GetOrAddBehaviour<Transform>();

	ModelRenderer* modelRdr;

	if (node->m_mesh)
	{
		modelRdr = entity->AddBehaviour<ModelRenderer>();
		modelRdr->m_transform = transform;
		modelRdr->SetMesh(node->m_mesh);
		modelRdr->SetMaterial(node->m_material);
		modelRdr->BindToSignals();

	}

	transform->SetPosition(node->m_baseTRS[0]);
	transform->SetRotation(node->m_baseTRS[1]);
	transform->SetScale(node->m_baseTRS[2]);

	transform->BindToSignals();

	if (parentEntity)
	{
		Transform* parentTransform = parentEntity->GetOrAddBehaviour<Transform>();
		transform->SetParent(parentTransform);
	}

	entities.push_back(entity);

	for (ModelNode* childNode : node->m_childrenNode)
		GenerateEntitiesRecursive(childNode, entity, entities);
}

void Scene::GenerateEntities(std::shared_ptr<ModelBase> modelBase)
{
	std::vector<Entity*> entities;

	ModelNode* rootNode = modelBase->GetRootNode();

	if (modelBase->GetMeshCount() && rootNode)
		GenerateEntitiesRecursive(rootNode, nullptr, entities);

	for (Entity* entity : entities)
	{
		AddEntity(entity);
		entity->Initialize();
	}
}

Entity* Scene::FindEntity(uint32_t id)
{
	// TODO: Change this to unordered map of all uuid containers
	for (Entity* entity : m_entities)
	{
		if ((uint32_t)entity->GetUUID() == id)
			return entity;
	}

	return nullptr;
}

Entity* Scene::FindModelEntity(uint32_t id)
{
	for (Entity* entity : m_entities)
	{
		if (ModelRenderer* rdr = entity->GetBehaviour<ModelRenderer>(); rdr && rdr->m_id == id)
			return entity;
	}
	return nullptr;
}

void Scene::Empty() 
{
	while(m_entities.size() > 0)
	{
		m_entities[0]->Destroy();
		m_entities.erase(m_entities.begin());
	}
}