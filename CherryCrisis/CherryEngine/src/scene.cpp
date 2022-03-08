#include "pch.hpp"

#include "scene.hpp"

#include "resource_manager.hpp"
#include "render_manager.hpp"

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

Resource::Ref<Scene> Scene::Create(const char* filePath)
{
	Scene* scene = new Scene(filePath);

	auto RM = ResourceManager::GetInstance();

	std::shared_ptr<ModelBase> modelBase = RM->AddResource<ModelBase>("../assets/backpack.obj", true);

	Entity* root = new Entity("Root");
	std::vector<Entity*> children = modelBase->GenerateEntities(root);
	root->AddBehaviour<ScriptedBehaviour>();

	scene->AddEntity(root);

	for (Entity* child : children)
		scene->AddEntity(child);

	Entity* light = new Entity("Light");
	light->AddBehaviour<LightComponent>();
	scene->AddEntity(light);

	Entity* camera = new Entity("Camera");
	camera->AddBehaviour<CameraComponent>()->m_transform = camera->AddBehaviour<Transform>();

	scene->AddEntity(camera);

	return Ref<Scene>(scene);
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
