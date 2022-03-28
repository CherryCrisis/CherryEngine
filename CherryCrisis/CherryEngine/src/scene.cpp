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
#include "model.hpp"

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
	camera->AddBehaviour<ScriptedBehaviour>()->SetScriptClass("CameraController");

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

	root->AddBehaviour<ScriptedBehaviour>()->SetScriptClass("BackpackBehaviour");

	AddEntity(root);

	for (Entity* child : children)
		AddEntity(child);

	//auto func = CCFunction::BindFunction(&ResourceManager::Remove<ModelBase>, ResourceManager::GetInstance(), resource->GetFilepath());
	//ThreadPool::GetInstance()->CreateTask(func, EChannelTask::MAINTHREAD);
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
			myfile << m_entity.second->SerializeBehaviours();
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

Entity* Scene::FindEntity(uint64_t id) 
{
	for (const auto& entity : m_entities)
	{
		if ((uint64_t)entity.second->GetUUID() == id)
			return entity.second;

	}
	return nullptr;
}

std::string ExtractValue(std::string str, const char key = ':')
{
	return str.substr(str.find_first_of(key) + 1);
}

std::string ExtractKey(std::string& str, const char key = ':', bool erase = false)
{
	std::string strr = str.substr(0, str.find(key));

	if (erase) 
	{
		std::string::size_type i = str.find(strr);

		if (i != std::string::npos)
			str.erase(i, strr.length() + 1);
	}

	return strr;
}

uint64_t ExtractUUID(const std::string& str) 
{
	std::string uuid = ExtractValue(str);

	uint64_t value;
	std::istringstream iss(uuid);
	iss >> value;
	return value;
}

float ExtractFloat(const std::string& str)
{
	std::string vector = ExtractValue(str);
	return std::stof(vector);
}

int ExtractInt(const std::string& str)
{
	std::string vector = ExtractValue(str);
	return std::stoi(vector);
}

//Send the value and it parses it
CCMaths::Vector3 ExtractVector3(std::string str)
{	
	CCMaths::Vector3 value{};
	std::string temp = ExtractKey(str, '/', true);
	value.x = std::stof(temp);
	temp = ExtractKey(str, '/', true);
	value.y = std::stof(temp);
	temp = ExtractKey(str, '/', true);
	value.z = std::stof(temp);
	return value;
}




bool Scene::Unserialize(const char* filePath) 
{
	for (auto& [name, ref] : m_entities) 
		delete ref;

	m_entities.clear();

	//First read the file and populate the entities + a uuid map and the component wrapper list 
	std::string fileName = "Assets/" + std::string(filePath) + ".cherry";
	std::ifstream file(fileName);

	// first is the uuid and the behaviour pointer 
	std::unordered_map<uint64_t, Behaviour*> m_wrappedBehaviours;
	// first is the behaviour uuidand the uuid to link in 
	std::unordered_map<uint64_t, std::unordered_map<std::string, uint64_t>> m_wrappedUUIDs;

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
		bool isParsingComponent = false;
		Behaviour* behaviour = nullptr;

		// need to get the actual serialized uuid 
		uint64_t value;
		uint64_t parent;

		while (std::getline(buffer, line))
		{
			found = line.find("Entity:");
			if (found != std::string::npos)
			{
				isParsingComponent = false;
				value = ExtractUUID(line);
				Entity* empty = new Entity("Empty", CCUUID(value));
				
				AddEntity(empty);
				continue;
			}

			found = line.find("Behaviour:");
			if (found != std::string::npos)
			{
				isParsingComponent = true;
				value = ExtractUUID(line);
			}
			else if (isParsingComponent)
			{
				found = line.find("Transform");
				if (found != std::string::npos)
				{
					behaviour = new Transform();
				}
				found = line.find("LightComponent");
				if (found != std::string::npos)
				{
					behaviour = new LightComponent();
				}
				found = line.find("ModelRenderer");
				if (found != std::string::npos)
				{
					behaviour = new ModelRenderer();
				}

				if (!behaviour)
					continue;

				std::string key = ExtractKey(line);
				std::string parsedValue = ExtractValue(line);

				if (line.size() == 0)
				{
					behaviour->m_uuid = value;
					m_wrappedBehaviours[value] = behaviour;
					behaviour = nullptr;
					continue;
				}

				if (key == "m_owner")
				{
					if (behaviour)
						behaviour->m_parentUuid = ExtractUUID(line);
				}

				if (!behaviour->m_metadatas.m_fields.contains(key))
					continue;

				auto& info = behaviour->m_metadatas.m_fields[key].m_value.type();
				if (info == typeid(CCMaths::Vector3**))
				{

					Vector3* vec = new Vector3();
					*vec = ExtractVector3(parsedValue);

					behaviour->m_metadatas.m_fields[key] = { key, std::any(vec) };
				}

				if (info == typeid(CCMaths::Vector3*))
				{
					Vector3 vec = {};
					behaviour->m_metadatas.m_fields[key] = { key, std::any(vec) };
				}

				if (info == typeid(Behaviour**))
				{
					uint64_t refUUID = ExtractUUID(line);
					if (refUUID != 0)
						m_wrappedUUIDs[value].insert({ key, refUUID });
				}

				if (info == typeid(std::string*))
				{
					behaviour->m_metadatas.m_fields[key] = { key, std::any(value) };
					
					ModelRenderer* renderer = (ModelRenderer*) behaviour;
					if (renderer) 
					{
						//std::shared_ptr<Model> model = ResourceManager::GetInstance()->AddResource<Model>(parsedValue.c_str(), true);
						//renderer->SetModel(model);
					}
				}


				//srd_ptr<model> model = AddResourceNoLoad(modelName)
				// 
				// {
				//	uniqueGuard(mutex)
				// 
				//	if (model is load) 
				//		modelRender.SetModel(model);
				//	else
				//		model.onLoaded.Bind(&ModelRenderer::SetModel, this);
				// }
				//
			}
		}
		//Then loop over the wrapped component to add them into the entities
		for (auto& wrappedBehaviour : m_wrappedBehaviours) 
		{
			Entity* entity = FindEntity(wrappedBehaviour.second->GetOwnerUUID());
			if (entity)
				entity->SubscribeComponent(wrappedBehaviour.second);
		}

		//Then loop over the wrapped component again to link the uuids
		for (const auto& [UUID, behaviourRef] : m_wrappedBehaviours)
		{
			auto grave = m_wrappedUUIDs.find(UUID);
			if (grave == m_wrappedUUIDs.end())
				continue;

			for (auto& [fieldName, fieldRef] : behaviourRef->m_metadatas.m_fields)
			{
				if (fieldRef.m_value.type() == typeid(Behaviour*)) 
				{
					auto refIt = grave->second.find(fieldName);

					if (refIt == grave->second.end())
						continue;

					uint64_t refUUID = refIt->second;

					auto behaviourIt = m_wrappedBehaviours.find(refUUID);

					if (behaviourIt == m_wrappedBehaviours.end())
						continue;

					Behaviour* bhave = behaviourIt->second;
					fieldRef.m_value = bhave;
				}
			}
			behaviourRef->ConsumeMetadatas();
		}
	}


	return opened;
}