#include <pch.hpp>

#include <serializer.hpp>
#include <format>

#include "resource_manager.hpp"
#include "utils.hpp"

#include "transform.hpp"
#include "light_component.hpp"
#include "model_renderer.hpp"
#include "camera_component.hpp"
#include "scripted_behaviour.hpp"
#include "model.hpp"

#include "scene_manager.hpp"
#include "object.hpp"
#include "yaml-cpp/yaml.h"

// to remove
void Foos(std::shared_ptr<ModelBase>)
{

}

namespace YAML {
	template<>
	struct convert<CCMaths::Vector3> {
		static Node encode(const CCMaths::Vector3& rhs)
		{
			Node node;
			node = (std::to_string(rhs.x) + '/' + std::to_string(rhs.y) + '/' + std::to_string(rhs.z));
			return node;
		}

		static bool decode(const Node& node, CCMaths::Vector3& rhs)
		{
			rhs = String::ExtractVector3(node.as<std::string>());
			return true;
		}
	};

	template<>
	struct convert<Object> {
		static Node encode(const Object& rhs)
		{
			Node node;
			return node = rhs.GetUUID();
			//return node = rhs ? rhs->GetUUID() : '~';
		}

		static bool decode(const Node& node, Object& rhs)
		{
			//rhs = SceneManager::GetInstance()->
			return true;
		}
	};
}


bool Serializer::SerializeScene(Scene* scene, const char* filepath) 
{
	YAML::Node save;
	YAML::Node ressources;

	std::map<std::type_index, std::vector<std::filesystem::path*>> pathList;
	ResourceManager::GetInstance()->GetResourcesPath(pathList);

	//Resources Saving
	for (const auto& [type, paths] : pathList)
	{
		if (type == typeid(ModelBase))
			for (const auto& path : paths)
				save["resources"].push_back(path->string());
	}

	//Entities Saving
	for (const auto& [eName, eRef] : scene->m_entities) 
	{
		YAML::Node entity;
		save["entities"][eRef->GetUUID()]["name"] = eRef->GetName();
	}

	const char* parseError = "#ERROR {}  is not handled !";//std::string("#ERROR# ") + std::string(type.name()) + std::string(" is not handled !") + "\n";
	//Components Saving
	for (const auto& [eName, eRef] : scene->m_entities)
	{
		for (const auto& behaviour : eRef->GetAllBehaviours()) 
		{	
			uint32_t UUID = behaviour->GetUUID();
			save["components"][UUID]["type"] = String::ExtractValue(typeid(*behaviour).name(), ' ');
			// Field Saving
			for (const auto& [fieldName, fieldRef] : behaviour->m_metadatas.m_fields)
			{
				const std::type_index& type = fieldRef.m_type;

				if (type == typeid(CCMaths::Vector3))
					save["components"][UUID][fieldName] = *std::any_cast<CCMaths::Vector3*>(fieldRef.m_value);

				else if (type == typeid(std::string))
					save["components"][UUID][fieldName] = *std::any_cast<std::string*>(fieldRef.m_value);

				else if (type == typeid(float))
					save["components"][UUID][fieldName] = *std::any_cast<float*>(fieldRef.m_value);

				else if (type == typeid(int))
					save["components"][UUID][fieldName] = *std::any_cast<int*>(fieldRef.m_value);

				else if (type == typeid(bool))
					save["components"][UUID][fieldName] = *std::any_cast<bool*>(fieldRef.m_value);

				else if (type == typeid(Object*)) 
				{
					Object* obj = *std::any_cast<Object**>(fieldRef.m_value);
					save["components"][UUID][fieldName] = obj ? YAML::Node(*obj) : YAML::Node('~');
				}
					

				else //Unhandled Cases (useful to find them)
					save["components"][UUID][fieldName] = std::format(parseError, type.name());
			}

			//Properties saving
			for (const auto& [propName, propRef] : behaviour->m_metadatas.m_properties)
			{
				const std::type_index& type = propRef->GetGetType();

				if (type == typeid(CCMaths::Vector3))
				{
					CCMaths::Vector3 val;
					propRef->Get(&val);
					save["components"][UUID][propName] = val;
				}
				else if (type == typeid(std::string))
				{
					std::string val;
					propRef->Get(&val);
					save["components"][UUID][propName] = val;
				}
				else if (type == typeid(float))
				{
					float val;
					propRef->Get(&val);
					save["components"][UUID][propName] = val;
				}
				else if (type == typeid(int))
				{
					int val;
					propRef->Get(&val);
					save["components"][UUID][propName] = val;
				}
				else if (type == typeid(bool))
				{
					bool val;
					propRef->Get(&val);
					save["components"][UUID][propName] = val;
				}
				else if (type == typeid(Object*) || type == typeid(Entity*) || type == typeid(Transform*))
				{
					Object* ptr;
					propRef->Get(&ptr);
					save["components"][UUID][propName] = ptr ? YAML::Node(*ptr) : YAML::Node();
				}
				else //Unhandled Cases (useful to find them)
					save["components"][UUID][propName] = std::format(parseError, type.name());
			}
		}
	}
	std::string fileName = strlen(filepath) > 0 ? std::string(filepath) : scene->GetFilepath();
	std::ofstream out(fileName);

	bool opened = out.is_open();
	out << save;
	out.close();

	return opened;
}

bool Serializer::CreateBehaviour(const std::string& type, uint32_t uuid, Behaviour** outBehaviour)
{
	CCUUID id(uuid);

	if      (type == "Transform")	      *outBehaviour = new Transform(id);
	else if (type == "LightComponent")    *outBehaviour = new LightComponent(id);
	else if (type == "ModelRenderer")     *outBehaviour = new ModelRenderer(id);
	else if (type == "ScriptedBehaviour") *outBehaviour = new ScriptedBehaviour(id);
	else if (type == "CameraComponent")   *outBehaviour = new CameraComponent(id);

	return *outBehaviour;
}

bool Serializer::UnserializeScene(std::shared_ptr<Scene> scene, const char* filepath)
{
	// First clean the scene container
	scene->Empty();

	std::string fileName = strlen(filepath) > 0 ? std::string(filepath) : scene->GetFilepath();
	
	// first is the uuid and the behaviour pointer 
	std::unordered_map<uint32_t, Object*> m_wrappedBehaviours;
	// first is the behaviour uuid and the uuid to link in 
	std::unordered_map<uint32_t, std::unordered_map<std::string, uint32_t>> m_wrappedUUIDs;

	// Then unpack the YAML
	YAML::Node loader = YAML::LoadFile(fileName);


	// Iterates on Resources  entries
	if (loader["resources"]) 
	{
		for (auto resource : loader["resources"])
		{
			std::string str = resource.as<std::string>();
			const char* Cstr = str.c_str(); // need to casts it as a const char for MultiThread
			ResourceManager::GetInstance()->AddResourceMultiThreads<ModelBase>(Cstr, true, nullptr);
		}
	}


	// Iterates on Entities entries
	if (loader["entities"])
	{
		for (YAML::const_iterator it = loader["entities"].begin(); it != loader["entities"].end(); ++it)
		{
			Entity* empty = new Entity(it->second["name"].as<std::string>(), it->first.as<uint32_t>());
			scene->AddEntity(empty);
		}
	}

	// Iterates on Components entries
	if (loader["components"])
	{
		for (YAML::const_iterator it = loader["components"].begin(); it != loader["components"].end(); ++it)
		{
			Behaviour** b = nullptr;

			//Create Behaviour by its type,
			if (!CreateBehaviour(it->second["type"].as<std::string>(), it->first.as<uint32_t>(), b))
				continue;

			Behaviour* behaviour = *b;

			//Set owner by sceneManager raw 
			for (YAML::const_iterator entry = it->second.begin(); entry != it->second.end(); ++entry)
			{
				std::string key = entry->first.as<std::string>();
				YAML::Node value = entry->second;

				if (behaviour->m_metadatas.m_properties.contains(key)) 
				{
					CCProperty::IClearProperty* prop = behaviour->m_metadatas.m_properties[key];

					auto& propType = prop->GetGetType();
					
					if (propType == typeid(Transform*) || propType == typeid(Entity*))
					{
						m_wrappedUUIDs[it->first.as<uint32_t>()].insert({ key, it->second.as<uint32_t>()});
					}
					else if (propType == typeid(CCMaths::Vector3))
					{
						Vector3 vec = value.as<CCMaths::Vector3>();
						prop->Set(&vec);
					}
					else if (propType == typeid(std::string))
					{
						std::string str = value.as<std::string>();
						prop->Set(&str);
					}
					else if (propType == typeid(bool))
					{
						bool str = value.as<bool>();
						prop->Set(&str);
					}
				}

				if (behaviour->m_metadatas.m_fields.contains(key))
				{
					auto& info = behaviour->m_metadatas.m_fields[key].m_type;
					Field& field = behaviour->m_metadatas.m_fields[key];

					if (info == typeid(CCMaths::Vector3))
					{
						CCMaths::Vector3* valPtr = std::any_cast<CCMaths::Vector3*>(field.m_value);
						*valPtr = value.as<CCMaths::Vector3>();
					}
					else if (info == typeid(bool))
					{
						bool* valPtr = std::any_cast<bool*>(field.m_value);
						*valPtr = value.as<bool>();
					}
					else if (info == typeid(Object*))
					{
						m_wrappedUUIDs[it->first.as<uint32_t>()].insert({ key, it->second.as<uint32_t>() });
					}
					else if (info == typeid(std::string))
					{
						std::string* valPtr = std::any_cast<std::string*>(field.m_value);
						*valPtr = value.as<std::string>();

						ModelRenderer* renderer = (ModelRenderer*)behaviour;
						if (renderer)
						{
							std::shared_ptr<Model> model = ResourceManager::GetInstance()->AddResourceRef<Model>(value.as<std::string>().c_str());
							if (model->GetResourceState() == EResourceState::LOADED)
								renderer->SetModel(model);
							else
								model->m_OnLoaded.Bind(&ModelRenderer::SetModel, renderer);
						}
					}
				}
			}
		}
	}
	// Do the links
	//Then loop over the wrapped component to add them into the entities
	for (auto& wrappedBehaviour : m_wrappedBehaviours)
	{
		//if (Entity* entity = scene->FindEntity(wrappedBehaviour.second->GetOwnerUUID()))
		//	entity->SubscribeComponent(wrappedBehaviour.second);
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
			if (info == typeid(Object*))
			{
				//find the field in the UUID list of the behaviour
				auto refIt = grave->second.find(fieldName);

				if (refIt == grave->second.end())
					continue;

				//finally find the behaviour in the behaviour list by its uuid
				auto behaviourIt = m_wrappedBehaviours.find(refIt->second);

				if (behaviourIt == m_wrappedBehaviours.end())
					continue;

				Object** valuePtr = std::any_cast<Object**>(fieldRef.m_value);

				Object* bhave = behaviourIt->second;

				*valuePtr = bhave;
			}
		}

		//Loop over the fields of the behaviour
		for (auto& [propName, propRef] : behaviourRef->m_metadatas.m_properties)
		{
			auto& propType = propRef->GetGetType();

			if (propType == typeid(Entity*) || propType == typeid(Transform*))
			{
				//find the field in the UUID list of the behaviour
				auto refIt = grave->second.find(propName);

				if (refIt == grave->second.end())
					continue;

				//finally find the behaviour in the behaviour list by its uuid
				auto behaviourIt = m_wrappedBehaviours.find(refIt->second);

				if (behaviourIt == m_wrappedBehaviours.end())
					continue;

				Object* bhave = behaviourIt->second;
				propRef->Set(&bhave);
			}
		}
	}
	for (auto& [entityName, entityRef] : scene->m_entities)
	{
		entityRef->Initialize();
	}

	/*






	

	std::ifstream file(fileName);

	// first is the uuid and the behaviour pointer 
	std::unordered_map<uint32_t, Object*> m_wrappedBehaviours;
	// first is the behaviour uuid and the uuid to link in 
	std::unordered_map<uint32_t, std::unordered_map<std::string, uint32_t>> m_wrappedUUIDs;

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
		uint32_t actualUUID;
		uint32_t parent;

		while (std::getline(buffer, line))
		{
			std::string key = String::ExtractKey(line);
			std::string parsedValue = String::ExtractValue(line);

			if (key == "Res") 
			{
				const char* value_Cstr = parsedValue.c_str();
				auto callback = CCCallback::BindCallback(Foos);
				ResourceManager::GetInstance()->AddResourceMultiThreads<ModelBase>(value_Cstr, true, callback);
				continue;
			}

			found = line.find("Entity:");
			if (found != std::string::npos)
			{
				isParsingComponent = false;
				actualUUID = (uint32_t)String::ExtractUUID(line);

				std::getline(buffer, line);
				std::string name = String::ExtractValue(line);
				Entity* empty = new Entity(name, CCUUID(actualUUID));

				m_wrappedBehaviours[actualUUID] = empty;
				scene->AddEntity(empty);

				continue;
			}

			found = line.find("Behaviour:");
			if (found != std::string::npos)
			{
				isParsingComponent = true;
				actualUUID = (uint32_t)String::ExtractUUID(line);
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

				if (line.size() == 0)
				{
					behaviour->m_uuid = actualUUID;
					m_wrappedBehaviours[actualUUID] = behaviour;
					behaviour = nullptr;
					continue;
				}

				if (key == "m_owner")
				{
					//if (behaviour)
					//	behaviour->m_owner = (uint32_t)String::ExtractUUID(line);
				}

				if (behaviour->m_metadatas.m_properties.contains(key))
				{
					CCProperty::IClearProperty* prop = behaviour->m_metadatas.m_properties[key];

					auto& propType = prop->GetGetType();

					if (propType == typeid(Transform*) || propType == typeid(Entity*))
					{
						uint32_t refUUID = (uint32_t)String::ExtractUUID(line);
						if (refUUID != 0)
							m_wrappedUUIDs[actualUUID].insert({ key, refUUID });

						continue;
					}

					if (propType == typeid(CCMaths::Vector3))
					{
						Vector3 vec = String::ExtractVector3(parsedValue);
						prop->Set(&vec);

						continue;
					}

					if (propType == typeid(std::string))
					{
						std::string str = String::ExtractValue(parsedValue);
						prop->Set(&str);

						continue;
					}

					if (propType == typeid(bool))
					{
						bool str = String::ExtractInt(parsedValue);
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
						*valPtr = String::ExtractVector3(parsedValue);
						continue;
					}

					if (info == typeid(bool))
					{
						bool* valPtr = std::any_cast<bool*>(field.m_value);
						*valPtr = String::ExtractInt(parsedValue);
						continue;
					}

					if (info == typeid(Object*))
					{
						uint32_t refUUID = (uint32_t)String::ExtractUUID(line);
						if (refUUID != 0)
							m_wrappedUUIDs[actualUUID].insert({ key, refUUID });

						continue;
					}

					if (info == typeid(std::string))
					{
						std::string* valPtr = std::any_cast<std::string*>(field.m_value);
						*valPtr = parsedValue;

						ModelRenderer* renderer = (ModelRenderer*)behaviour;
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
			//if (Entity* entity = scene->FindEntity(wrappedBehaviour.second->GetOwnerUUID()))
			//	entity->SubscribeComponent(wrappedBehaviour.second);
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
				if (info == typeid(Object*))
				{
					//find the field in the UUID list of the behaviour
					auto refIt = grave->second.find(fieldName);

					if (refIt == grave->second.end())
						continue;

					//finally find the behaviour in the behaviour list by its uuid
					auto behaviourIt = m_wrappedBehaviours.find(refIt->second);

					if (behaviourIt == m_wrappedBehaviours.end())
						continue;

					Object** valuePtr = std::any_cast<Object**>(fieldRef.m_value);

					Object* bhave = behaviourIt->second;

					*valuePtr = bhave;
				}
			}
			
			//Loop over the fields of the behaviour
			for (auto& [propName, propRef] : behaviourRef->m_metadatas.m_properties)
			{
				auto& propType = propRef->GetGetType();

				if (propType == typeid(Entity*) || propType == typeid(Transform*))
				{
					//find the field in the UUID list of the behaviour
					auto refIt = grave->second.find(propName);

					if (refIt == grave->second.end())
						continue;

					//finally find the behaviour in the behaviour list by its uuid
					auto behaviourIt = m_wrappedBehaviours.find(refIt->second);

					if (behaviourIt == m_wrappedBehaviours.end())
						continue;

					Object* bhave = behaviourIt->second;
					propRef->Set(&bhave);
				}
			}
		}
	}
	for (auto& [entityName, entityRef] : scene->m_entities) 
	{

		entityRef->Initialize();
	}
	*/
	return true;
}

bool Serializer::SerializeEditor(const char* filepath)
{
	std::ofstream myfile;
	std::string fileName;
	fileName = std::string(filepath);

	bool opened = false;
	myfile.open(fileName);
	if (myfile.is_open())
	{
		//Write Editor save infos

		myfile.close();
		opened = true;
	}

	return opened;
}

bool Serializer::SerializeGame(const char* filepath)
{
	std::ofstream myfile;
	std::string fileName;
	fileName = std::string(filepath);

	bool opened = false;
	myfile.open(fileName);
	if (myfile.is_open())
	{
		//Write Game Infos such as scenes
		myfile << "MainScene:";
		myfile << SceneManager::GetInstance()->m_currentScene->GetFilepath() << std::endl;

		myfile.close();
		opened = true;
	}

	return opened;
}

bool Serializer::UnserializeGame(const char* filepath) 
{
	std::ifstream file(filepath);

	bool opened = false;
	if (file)
	{
		std::stringstream buffer;
		std::string line;
		std::size_t found;

		buffer << file.rdbuf();
		file.close();

		while (std::getline(buffer, line))
		{
			found = line.find("MainScene:");
			if (found != std::string::npos)
				SceneManager::LoadScene(String::ExtractValue(line).c_str());
		}
		opened = true;
	}
	
	return opened;
}