#include <pch.hpp>

#include <serializer.hpp>

#include "resource_manager.hpp"
#include "utils.hpp"

#include "transform.hpp"
#include "light_component.hpp"
#include "model_renderer.hpp"
#include "camera_component.hpp"
#include "scripted_behaviour.hpp"
#include "model.hpp"

#include "scene_manager.hpp"
// to remove
void Foos(std::shared_ptr<ModelBase>)
{

}

bool Serializer::SerializeScene(Scene* scene, const char* filepath) 
{
	std::ofstream myfile;
	std::string fileName;
	if (strlen(filepath) > 0)
		fileName = std::string(filepath);
	else
		fileName = scene->GetFilepath();

	bool opened = false;
	myfile.open(fileName);
	if (myfile.is_open())
	{
		std::map<std::type_index, std::vector<const char*>> pathList;
		ResourceManager::GetInstance()->GetResourcesPath(pathList);
		
		for (const auto& [type, paths] : pathList)
		{
			if (type == typeid(ModelBase))
				for (const auto& path : paths)
					myfile <<"Res:"<< path << std::endl;
		}

		for (const auto& m_entity : scene->m_entities)
			myfile << m_entity.second->Serialized() << std::endl;

		for (const auto& m_entity : scene->m_entities)
			myfile << m_entity.second->SerializeBehaviours();

		myfile.close();
		opened = true;
	}

	return opened;
}

bool Serializer::UnserializeScene(std::shared_ptr<Scene> scene, const char* filepath)
{
	scene->Empty();

	//First read the file and populate the entities + a uuid map and the component wrapper list 
	std::string fileName;
	
	if (strlen(filepath) > 0)
		fileName = std::string(filepath);
	else
		fileName = scene->GetFilepath();

	std::ifstream file(fileName);

	// first is the uuid and the behaviour pointer 
	std::unordered_map<uint32_t, Behaviour*> m_wrappedBehaviours;
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
					if (behaviour)
						behaviour->m_parentUuid = (uint32_t)String::ExtractUUID(line);
				}

				if (behaviour->m_metadatas.m_properties.contains(key))
				{
					CCProperty::IClearProperty* prop = behaviour->m_metadatas.m_properties[key];

					auto& propType = prop->GetGetType();

					if (propType == typeid(Transform*))
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

					if (info == typeid(Behaviour*))
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
			Entity* entity = scene->FindEntity(wrappedBehaviour.second->GetOwnerUUID());
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
	for (auto& [entityName, entityRef] : scene->m_entities)
		entityRef->Initialize();

	return opened;
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