#pragma once

#include <string>
#include <unordered_map>

#include "resource.hpp"
#include "entity.hpp"
#include "skybox.hpp"

class ModelBase;

class CCENGINE_API Scene : public Resource<Scene>
{
private:

public:
	Scene(const char* filePath) : Resource(filePath) { }
	virtual ~Scene();
	
	// TODO: Switch to unique_ptr
	std::unordered_map<std::string, Entity*> m_entities;

	Skybox m_skybox;

	//Call entities Initialize 
	void Initialize();
	
	//Call entities Update
	void Update();

	//To Move
	static void Load(std::shared_ptr<Scene> scene);
	bool Save(const char* sceneName);
	
	//To Rework
	void GenerateEntities(std::shared_ptr<ModelBase> modelBase);

	//To Rework
	std::string GetUniqueEntityName(const std::string& entityName);

	//To Rework
	void AddEntity(Entity* toAdd);
	//To Rework
	void RemoveEntity(Entity* toRemove);
	//To Rework
	void RemoveEntity(const std::string& name);
	//To Rework
	Entity* FindEntity(uint64_t id);

	//To Move
	bool Serialize(const char* filePath);
	bool Unserialize(const char* filePath);
};
