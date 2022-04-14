#pragma once

#include <string>
#include <unordered_map>

#include "resource.hpp"
#include "entity.hpp"
#include "skybox.hpp"

class ModelBase;

class CCENGINE_API Scene : public Resource<Scene>
{ 
public:
	
	Scene(const char* filePath) : Resource(filePath) { }
	virtual ~Scene();
	
	// TODO: Switch to unique_ptr this is shitty shit shit shit
	std::unordered_map<std::string, Entity*> m_entities;

	Skybox m_skybox;

	//Call entities Initialize 
	void Initialize();
	
	//Call entities Update
	void Update();

	//To Move
	static void Load(std::shared_ptr<Scene> scene);

	std::string GetUniqueEntityName(const std::string& entityName);

	void PopulateEmpty();

	//To Rework
	void AddEntity(Entity* toAdd);
	//To Rework
	void RemoveEntity(Entity* toRemove);
	//To Rework
	void RemoveEntity(const std::string& name);
	//To Rework
	Entity* FindEntity(uint32_t id);
	//To Remove
	Entity* FindModelEntity(uint32_t id);

	void GenerateEntities(std::shared_ptr<ModelBase> resource);

	std::string GetName() { return m_filepath; }

	bool Save();
	bool SaveAs(const char* filepath);

	void Empty();
};
