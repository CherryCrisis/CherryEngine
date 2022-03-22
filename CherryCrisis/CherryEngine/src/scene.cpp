#include "pch.hpp"

#include "scene.hpp"

#include <iostream>
#include <fstream>

#include "resource_manager.hpp"
#include "render_manager.hpp"
#include "callback.hpp"

// TODO: Remove this
#include "transform.hpp"
#include "light_component.hpp"
#include "model_renderer.hpp"
#include "camera_component.hpp"
#include "scripted_behaviour.hpp"

#include "model_base.hpp"

Scene::~Scene()
{
	for (auto [entityName, entityPtr] : m_entities)
		delete entityPtr;
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
	m_entities[GetUniqueEntityName(toAdd->GetName())] = toAdd;
}

void Scene::Load(std::shared_ptr<Scene> scene)
{
	auto RM = ResourceManager::GetInstance();

	auto callback = CCCallback::BindCallback(&Scene::GenerateEntities, scene.get());
	RM->AddResourceMultiThreads<ModelBase>("Assets/backpack.obj", true, callback);

	Entity* light = new Entity("Light");
	light->AddBehaviour<LightComponent>();
	scene->AddEntity(light);

	Entity* camera = new Entity("Camera");
	camera->AddBehaviour<CameraComponent>()->m_transform = camera->AddBehaviour<Transform>();

	scene->AddEntity(camera);
}

void Scene::GenerateEntities(std::shared_ptr<ModelBase> resource)
{
	std::shared_ptr<ModelBase> modelBase = std::dynamic_pointer_cast<ModelBase>(resource);

	Entity* root = new Entity("Root");
	std::vector<Entity*> children = modelBase->GenerateEntities(root);
	root->AddBehaviour<ScriptedBehaviour>();

	AddEntity(root);

	for (Entity* child : children)
		AddEntity(child);

	auto func = CCFunction::BindFunction(&ResourceManager::Remove<ModelBase>, ResourceManager::GetInstance(), resource->GetFilepath());
	ThreadPool::GetInstance()->CreateTask(func, EChannelTask::MAINTHREAD);
}

void Scene::Start()
{
	for (auto& [eName, entity] : m_entities)
		entity->Start();
}

void Scene::Update()
{
	for (auto& [eName, entity] : m_entities)
		entity->Update();
}

void Scene::Draw()
{
	RenderManager::DrawScene();
}

bool Scene::Serialize(const char* filePath) 
{
	std::ofstream myfile;
	std::string fileName = "Assets/" + std::string(filePath) + ".cherry";

	bool opened = false;
	myfile.open(fileName);
	if (myfile.is_open()) 
	{
		for (const auto& m_entity : m_entities) 
		{
			myfile << m_entity.second->Serialized() << std::endl;
		}
		myfile.close();
		opened = true;
	}
	else 
	{
		std::cout << "Unable to open file" << std::endl;
	}

	return opened;
}