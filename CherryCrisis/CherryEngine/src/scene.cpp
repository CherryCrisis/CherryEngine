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

void Scene::Load(const char* filepath) 
{
	// do things
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
		for (const auto& m_entity : m_entities)
		{
			myfile << m_entity.second->SerializeBehaviours() << std::endl;
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

bool Find(const std::string& string) 
{
	bool found = false;

	return found;
}

uint64_t ExtractUUID(const std::string str) 
{
	std::string uuid = str.substr(str.find(':') + 1);
	uint64_t value;
	std::istringstream iss(uuid);
	iss >> value;
	return value;
}


bool Scene::Unserialize(const char* filePath) 
{
	m_entities.clear();


	//First read the file and populate the entities + a uuid map and the component wrapper list 
	std::string fileName = "Assets/" + std::string(filePath) + ".cherry";
	std::ifstream file(fileName);

	std::unordered_map<uint64_t, Behaviour*> m_wrappedBehaviours;

	bool opened = false;
	if (file)
	{
		std::stringstream buffer;

		buffer << file.rdbuf();

		file.close();
		opened = true;

		// operations on the buffer...
		std::string line;
		std::size_t found;
		while (std::getline(buffer, line))
		{
			found = line.find("Entity:");
			if (found != std::string::npos)
			{
				uint64_t value = ExtractUUID(line);
				Entity* empty = new Entity("Empty", CCUUID(value));
				
				AddEntity(empty);
				continue;
			}

			found = line.find("Behaviour:");
			if (found != std::string::npos)
			{
				Behaviour* behaviour = new Behaviour();
				// need to get the actual serialized uuid 

				uint64_t value = ExtractUUID(line);
				m_wrappedBehaviours[value] = behaviour;
			}
		}
	}

	//Then loop over the wrapped component to add them into the entities
	// 
	// 
	//Then loop over the rapped component again to link the uuids
	return opened;
}