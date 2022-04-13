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
#include "model.hpp"

#include "serializer.hpp"

Scene::~Scene()
{
	for (auto [entityName, entityPtr] : m_entities)
		delete entityPtr;
}

void Scene::Initialize()
{
	for (auto& [eName, entity] : m_entities)
		entity->Initialize();
}

void Scene::Update()
{
 	for (auto& [eName, entity] : m_entities)
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

std::string Scene::GetUniqueEntityName(const std::string& entityName)
{
	// TODO: Try using string view
	std::string modifiedName = entityName;

	auto goIt = m_entities.find(modifiedName);

	for (int count = 1; goIt != m_entities.end(); count++, goIt = m_entities.find(modifiedName))
		modifiedName = entityName + std::to_string(count);

	return modifiedName;
}

void Scene::AddEntity(Entity* toAdd)
{
	std::string name = GetUniqueEntityName(toAdd->GetName());
	m_entities[name] = toAdd;
	toAdd->SetName(name);
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

	m_entities.erase(toRemove->GetName());
	toRemove->Destroy();
}

void Scene::RemoveEntity(const std::string& name)
{
	
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

void Scene::GenerateEntities(std::shared_ptr<ModelBase> resource)
{
	Entity* root = new Entity("Root");
	std::vector<Entity*> children = resource->GenerateEntities(root);
	AddEntity(root);

	for (Entity* child : children)
		AddEntity(child);
}

bool Find(const std::string& string) 
{
	bool found = false;

	return found;
}

Entity* Scene::FindEntity(uint32_t id)
{
	for (const auto& entity : m_entities)
	{
		if ((uint32_t)entity.second->GetUUID() == id)
			return entity.second;
	}

	return nullptr;
}

Entity* Scene::FindModelEntity(uint32_t id)
{
	for (const auto& [entityName, entityRef]: m_entities)
	{
		if (ModelRenderer* rdr = entityRef->GetBehaviour<ModelRenderer>(); rdr && rdr->m_id == id) 
			return entityRef;

	}
	return nullptr;
}

void Scene::Empty() 
{
	for (auto entityIt = m_entities.begin(); entityIt != m_entities.end(); entityIt = m_entities.erase(entityIt))
	{
		entityIt->second->Destroy();
	}
}