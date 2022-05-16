#include <pch.hpp>

#include <serializer.hpp>
#include <format>

#include "resource_manager.hpp"
#include "utils.hpp"

#include "entity.hpp"
#include "transform.hpp"
#include "light_component.hpp"
#include "model_renderer.hpp"
#include "camera_component.hpp"
#include "scripted_behaviour.hpp"
#include "rigidbody.hpp"
#include "box_collider.hpp"
#include "sphere_collider.hpp"
#include "capsule_collider.hpp"
#include "audio_emitter.hpp"
#include "audio_listener.hpp"
#include "model.hpp"
#include "character_controller.hpp"
#include "model_base.hpp"

#include "scene_manager.hpp"
#include "object.hpp"
#include "yaml-cpp/yaml.h"

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
		}
	};

	template<>
	struct convert<Bool3> {
		static Node encode(const Bool3& rhs)
		{
			Node node;
			node = (std::to_string(rhs.x) + '/' + std::to_string(rhs.y) + '/' + std::to_string(rhs.z));
			return node;
		}

		static bool decode(const Node& node, Bool3& rhs)
		{
			rhs = String::ExtractBool3(node.as<std::string>());
			return true;
		}
	};
}

bool Serializer::SerializeScene(Scene* scene, const char* filepath) 
{
	YAML::Node save;

	std::map<std::type_index, std::vector<std::filesystem::path*>> pathList;
	ResourceManager::GetInstance()->GetResourcesPath(pathList);

	YAML::Node resources = save["resources"];
	//Resources Saving
	for (const auto& [type, paths] : pathList)
	{
		if (type == typeid(ModelBase))
			for (const auto& path : paths)
				resources.push_back(path->string());
	}

	//Entities Saving
	YAML::Node entities = save["entities"];
	for (const Entity* entity : scene->m_entities) 
	{
		entities[entity->GetUUID()]["name"] = entity->GetName();
	}

	const char* parseError = "#ERROR {}  is not handled !";//std::string("#ERROR# ") + std::string(type.name()) + std::string(" is not handled !") + "\n";
	//Components Saving
	YAML::Node components = save["components"];
	for (Entity* entity : scene->m_entities)
	{
		std::vector<Behaviour*> behaviours = entity->GetAllBehaviours();
		for (const auto& behaviour : behaviours) 
		{	
			uint32_t UUID = behaviour->GetUUID();
			YAML::Node node = components[UUID];
			node["type"] = String::ExtractValue(typeid(*behaviour).name(), ' ');
			// Field Saving
			for (const auto& [fieldName, fieldRef] : behaviour->m_metadatas.m_fields)
			{
				const std::type_index& type = fieldRef.m_type;

				if (type == typeid(CCMaths::Vector3))
				{
					node[fieldName] = *std::any_cast<CCMaths::Vector3*>(fieldRef.m_value);
				 continue;
				}
				
				if (type == typeid(Bool3)) 
				{
					node[fieldName] = *std::any_cast<Bool3*>(fieldRef.m_value);
					continue;
				}

				if (type == typeid(std::string)) 
				{
					node[fieldName] = *std::any_cast<std::string*>(fieldRef.m_value);
					continue;
				}

				if (type == typeid(float)) 
				{
					node[fieldName] = *std::any_cast<float*>(fieldRef.m_value);
					continue;
				}

				if (type == typeid(int)) 
				{
					node[fieldName] = *std::any_cast<int*>(fieldRef.m_value);
					continue;
				}

				if (type == typeid(bool)) 
				{
					node[fieldName] = *std::any_cast<bool*>(fieldRef.m_value);
					continue;
				}

				if (type == typeid(Object*)) 
				{
					Object** objPtr = std::any_cast<Object**>(fieldRef.m_value);

					if (!objPtr) continue;
						
					Object* obj = *objPtr;
					node[fieldName] = obj ? YAML::Node(*obj) : YAML::Node();
					continue;
				}
				//Unhandled Cases (useful to find them)
					node[fieldName] = std::format(parseError, type.name());
			}

			//Properties saving
			for (const auto& [propName, propRef] : behaviour->m_metadatas.m_properties)
			{
				const std::type_index& type = propRef->GetGetType();

				if (type == typeid(CCMaths::Vector3))
				{
					CCMaths::Vector3 val;
					propRef->Get(&val);
					node[propName] = val;
					continue;
				}
				if (type == typeid(Bool3))
				{
					Bool3 val;
					propRef->Get(&val);
					node[propName] = val;
					continue;
				}
				if (type == typeid(std::string))
				{
					std::string val;
					propRef->Get(&val);
					node[propName] = val;
					continue;
				}
				if (type == typeid(float))
				{
					float val;
					propRef->Get(&val);
					node[propName] = val;
					continue;
				}
				if (type == typeid(int))
				{
					int val;
					propRef->Get(&val);
					node[propName] = val;
					continue;
				}
				if (type == typeid(bool))
				{
					bool val;
					propRef->Get(&val);
					node[propName] = val;
					continue;
				}
				if (type == typeid(const char*))
				{
					const char* val;
					propRef->Get(&val);
					node[propName] = std::string(val);
					continue;
				}
				if (type == typeid(Object*) || type == typeid(Entity*) || type == typeid(Transform*))
				{
					Object* ptr;
					propRef->Get(&ptr);
					node[propName] = ptr ? YAML::Node(*ptr) : YAML::Node();
					continue;
				}
				//Unhandled Cases (useful to find them)
					node[propName] = std::format(parseError, type.name());
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

Behaviour* Serializer::CreateBehaviour(const std::string& type, uint32_t uuid)
{
	CCUUID id(uuid);
	Behaviour* b = nullptr;

	if      (type == "Transform")	      b = new Transform(id);
	else if (type == "LightComponent")    b = new LightComponent(id);
	else if (type == "ModelRenderer")     b = new ModelRenderer(id);
	else if (type == "ScriptedBehaviour") b = new ScriptedBehaviour(id);
	else if (type == "CameraComponent")   b = new CameraComponent(id);
	else if (type == "Rigidbody")		  b = new Rigidbody(id); 
	else if (type == "BoxCollider")       b = new BoxCollider(id);
	else if (type == "SphereCollider")    b = new SphereCollider(id);
	else if (type == "CapsuleCollider")   b = new CapsuleCollider(id);
	else if (type == "AudioEmitter")      b = new AudioEmitter(id);
	else if (type == "AudioListener")     b = new AudioListener(id);
	else if (type == "CharacterController")	b = new CharacterController(id);
	
	return b;
}

bool Serializer::UnserializeScene(std::shared_ptr<Scene> scene, const char* filepath)
{
	// First clean the scene container
	scene->Empty();
	
	// first is the uuid and the behaviour pointer 
	std::unordered_map<uint32_t, Object*> m_wrappedBehaviours;
	// first is the behaviour uuid and the uuid to link in 
	std::unordered_map<uint32_t, std::unordered_map<std::string, uint32_t>> m_wrappedUUIDs;
	// Then unpack the YAML
	std::string fileName = strlen(filepath) > 0 ? std::string(filepath) : scene->GetFilepath();
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
			m_wrappedBehaviours[empty->GetUUID()] = empty;
		}
	}

	// Iterates on Components entries
	if (loader["components"])
	{
		for (YAML::const_iterator it = loader["components"].begin(); it != loader["components"].end(); ++it)
		{
			Behaviour* behaviourPtr = CreateBehaviour(it->second["type"].as<std::string>(), it->first.as<uint32_t>());

			//Create Behaviour by its type,
			if (!behaviourPtr)
				continue;
 
			for (YAML::const_iterator entry = it->second.begin(); entry != it->second.end(); ++entry)
			{
				std::string key = entry->first.as<std::string>();
				YAML::Node value = entry->second;

				if (value.as<std::string>() == "null")
					continue;

				if (behaviourPtr->m_metadatas.m_properties.contains(key))
				{
					CCProperty::IClearProperty* prop = behaviourPtr->m_metadatas.m_properties[key];

					auto& propType = prop->GetGetType();
					
					if (propType == typeid(Transform*) || propType == typeid(Entity*))
					{
						m_wrappedUUIDs[it->first.as<uint32_t>()].insert({ key, value.as<uint32_t>()});
					}
					else if (propType == typeid(CCMaths::Vector3))
					{
						Vector3 vec = value.as<CCMaths::Vector3>();
						prop->Set(&vec);
					}
					else if (propType == typeid(Bool3))
					{
						Bool3 vec = value.as<Bool3>();
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
					else if (propType == typeid(const char*))
					{
						std::string str = value.as<std::string>();
						const char* val = str.c_str();
						prop->Set(&val);
					}
				}

				if (behaviourPtr->m_metadatas.m_fields.contains(key))
				{
					auto& info = behaviourPtr->m_metadatas.m_fields[key].m_type;
					Field& field = behaviourPtr->m_metadatas.m_fields[key];

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
						m_wrappedUUIDs[it->first.as<uint32_t>()].insert({ key, value.as<uint32_t>() });
					}
					else if (info == typeid(std::string))
					{
						std::string* valPtr = std::any_cast<std::string*>(field.m_value);
						*valPtr = value.as<std::string>();
					}
				}
			}
			m_wrappedBehaviours[behaviourPtr->GetUUID()] = behaviourPtr;
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

	//for (auto& [entityName, entityRef] : scene->m_entities)
	//	entityRef->Initialize();

	return true;
}

bool Serializer::SerializeEditor(const char* filepath)
{
	// Better change all of this shit

	YAML::Node save;

	//Save scene
	save["General"]["LastOpenedScene"] = SceneManager::GetInstance()->m_currentScene->m_filepath.string();
	//Save project settings
	//Save preferences
	std::ofstream out("editor.meta");

	bool opened = out.is_open();
	out << save;
	out.close();

	return opened;
}

bool Serializer::UnserializeEditor(const char* filepath)
{

	if (!std::filesystem::exists(filepath)) 
	{
		SceneManager::LoadEmptyScene("Assets/Empty.cherry");
		return false;
	}

	YAML::Node loader = YAML::LoadFile(filepath);

	//Loads Scene
	if (loader["General"]["LastOpenedScene"] && std::filesystem::exists((loader["General"]["LastOpenedScene"].as<std::string>())))
	{
		SceneManager::LoadScene(loader["General"]["LastOpenedScene"].as<std::string>().c_str());
	}
	else 
	{
		SceneManager::LoadEmptyScene("Assets/Empty.cherry");
	}

	//Save project settings
	//Save preferences
	return true;
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
