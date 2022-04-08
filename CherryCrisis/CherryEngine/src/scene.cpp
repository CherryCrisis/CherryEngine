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


void Foo(std::shared_ptr<ModelBase>)
{
	std::cout << "charg� maggle comme ta 50" << std::endl;
}

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
	Transform* transform;
	if (!toRemove->TryGetBehaviour(transform))
		m_entities.erase(toRemove->GetName()); return;

	std::vector<Transform*> children = transform->GetChildren();

	for (int i = 0; i < children.size(); i++)
		RemoveEntity(&children[i]->GetHost());
	
	m_entities.erase(toRemove->GetName());
}

void Scene::RemoveEntity(const std::string& name)
{
	m_entities.erase(name);
}

void Scene::Load(std::shared_ptr<Scene> scene)
{
	//Replace all of this with unserialization
	//------------------------------------------------
	Entity* light = new Entity("Light");
	light->AddBehaviour<LightComponent>();
	scene->AddEntity(light);

	Entity* camera = new Entity("Camera");
	camera->AddBehaviour<Transform>();
	auto cameraComp = camera->AddBehaviour<CameraComponent>();
	cameraComp->BindToSignals();

	ScriptedBehaviour* bhave1 = camera->AddBehaviour<ScriptedBehaviour>();
	bhave1->SetScriptClass("CameraController"); bhave1->BindToSignals();
	scene->AddEntity(camera);
}

void Scene::GenerateEntities(std::shared_ptr<ModelBase> resource)
{
	Entity* root = new Entity("Root");
	std::vector<Entity*> children = resource->GenerateEntities(root);

	std::string backpack = "BackpackBehaviour";
	ScriptedBehaviour* bhave = root->AddBehaviour<ScriptedBehaviour>();
	bhave->SetScriptClass(backpack);
	bhave->BindToSignals();

	AddEntity(root);

	for (Entity* child : children)
		AddEntity(child);
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

	auto callback = CCCallback::BindCallback(Foo);
	ResourceManager::GetInstance()->AddResourceMultiThreads<ModelBase>("Assets/backpack.obj", true, callback);

	//First read the file and populate the entities + a uuid map and the component wrapper list 
	std::string fileName = "Assets/" + std::string(filePath);
	std::ifstream file(fileName);

	// first is the uuid and the behaviour pointer 
	std::unordered_map<uint64_t, Behaviour*> m_wrappedBehaviours;
	// first is the behaviour uuid and the uuid to link in 
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
		uint64_t actualUUID;
		uint64_t parent;

		while (std::getline(buffer, line))
		{
			found = line.find("Entity:");
			if (found != std::string::npos)
			{
				isParsingComponent = false;
				actualUUID = ExtractUUID(line);

				std::getline(buffer, line);
				std::string name = ExtractValue(line);
				Entity* empty = new Entity(name, CCUUID(actualUUID));

				AddEntity(empty);

				continue;
			}

			found = line.find("Behaviour:");
			if (found != std::string::npos)
			{
				isParsingComponent = true;
				actualUUID = ExtractUUID(line);
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
				found = line.find("ScriptedBehaviour");
				if (found != std::string::npos)
				{
					behaviour = new ScriptedBehaviour();
				}
				found = line.find("CameraComponent");
				if (found != std::string::npos)
				{
					behaviour = new CameraComponent();
				}

				if (!behaviour)
					continue;

				std::string key = ExtractKey(line);
 				std::string parsedValue = ExtractValue(line);

				if (line.size() == 0)
				{
					behaviour->m_uuid = actualUUID;
					m_wrappedBehaviours[actualUUID] = behaviour;
					behaviour = nullptr;
					continue;
				}

				if (key == "m_owner")
				{
					if (behaviour)
						behaviour->m_parentUuid = ExtractUUID(line);
				}

				if (behaviour->m_metadatas.m_properties.contains(key))
				{
					CCProperty::IClearProperty* prop = behaviour->m_metadatas.m_properties[key];

					auto& propType = prop->GetGetType();

					if (propType == typeid(Transform*))
					{
						uint64_t refUUID = ExtractUUID(line);
						if (refUUID != 0)
							m_wrappedUUIDs[actualUUID].insert({ key, refUUID });

						continue;
					}

					if (propType == typeid(CCMaths::Vector3))
					{
						Vector3 vec = ExtractVector3(parsedValue);
						prop->Set(&vec);

						continue;
					}

					if (propType == typeid(std::string))
					{
						std::string str = ExtractValue(parsedValue);
						prop->Set(&str);

						continue;
					}
				}

				if (behaviour->m_metadatas.m_fields.contains(key))
				{
					auto& info = behaviour->m_metadatas.m_fields[key].m_type;

					Field& field = behaviour->m_metadatas.m_fields[key];

					if (info == typeid(CCMaths::Vector3))
					{
						CCMaths::Vector3* valPtr = std::any_cast<CCMaths::Vector3*>(field.m_value);
						*valPtr = ExtractVector3(parsedValue);
						continue;
					}

					if (info == typeid(Behaviour*))
					{
						uint64_t refUUID = ExtractUUID(line);
						if (refUUID != 0)
							m_wrappedUUIDs[actualUUID].insert({ key, refUUID });

						continue;
					}

					if (info == typeid(std::string))
					{
						std::string* valPtr = std::any_cast<std::string*>(field.m_value);
						*valPtr = parsedValue;

						ModelRenderer* renderer = (ModelRenderer*) behaviour;
						if (renderer) 
						{
							std::shared_ptr<Model> model = ResourceManager::GetInstance()->AddResourceRef<Model>(parsedValue.c_str());
							if (model->GetResourceState() == EResourceState::LOADED)
								renderer->SetModel(model);
							else
								model->m_OnLoaded.Bind(&ModelRenderer::SetModel, renderer);
						}

						continue;
					}
				}

			}
		}

		//Then loop over the wrapped component to add them into the entities
		for (auto& wrappedBehaviour : m_wrappedBehaviours) 
		{
			Entity* entity = FindEntity(wrappedBehaviour.second->GetOwnerUUID());
			if (entity) 
			{
				entity->SubscribeComponent(wrappedBehaviour.second);
				wrappedBehaviour.second->BindToSignals();
			}
		}

		//Then loop over the wrapped component again to link the uuids
		for (const auto& [UUID, behaviourRef] : m_wrappedBehaviours)
		{
			//Find the UUID list of the actual behaviour
			auto grave = m_wrappedUUIDs.find(UUID);
			if (grave == m_wrappedUUIDs.end())
				continue;

			//Loop over the fields of the behaviour
			for (auto& [fieldName, fieldRef] : behaviourRef->m_metadatas.m_fields)
			{
				auto& info = fieldRef.m_type;
				if (info == typeid(Behaviour*))
				{
					//find the field in the UUID list of the behaviour
					auto refIt = grave->second.find(fieldName);

					if (refIt == grave->second.end())
						continue;

					//finally find the behaviour in the behaviour list by its uuid
					auto behaviourIt = m_wrappedBehaviours.find(refIt->second);

					if (behaviourIt == m_wrappedBehaviours.end())
						continue;

					Behaviour** valuePtr = std::any_cast<Behaviour**>(fieldRef.m_value);

					Behaviour* bhave = behaviourIt->second;
					
					*valuePtr = bhave;
				}
			}

			//Loop over the fields of the behaviour
			for (auto& [propName, propRef] : behaviourRef->m_metadatas.m_properties)
			{
				auto& propType = propRef->GetGetType();

				if (propType == typeid(Transform*))
				{
					//find the field in the UUID list of the behaviour
					auto refIt = grave->second.find(propName);

					if (refIt == grave->second.end())
						continue;

					//finally find the behaviour in the behaviour list by its uuid
					auto behaviourIt = m_wrappedBehaviours.find(refIt->second);

					if (behaviourIt == m_wrappedBehaviours.end())
						continue;

					Behaviour* bhave = behaviourIt->second;
					propRef->Set(&bhave);
				}
			}
		}
	}
	for (auto& [entityName, entityRef] : m_entities)
		entityRef->Initialize();
	
	return opened;
}
