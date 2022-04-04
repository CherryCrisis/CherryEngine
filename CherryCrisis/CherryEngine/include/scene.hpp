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

	Skybox m_skybox;

	// TODO: Switch to unique_ptr
	std::unordered_map<std::string, Entity*> m_entities;
	static void Load(std::shared_ptr<Scene> scene);
	void GenerateEntities(std::shared_ptr<ModelBase> modelBase);
	void Start();
	void Update();

	void Draw();

	std::string GetUniqueEntityName(const std::string& entityName);

	void AddEntity(Entity* toAdd);
	void RemoveEntity(Entity* toRemove);
	void RemoveEntity(std::string name);
	Entity* FindEntity(uint64_t id);

	bool Serialize(const char* filePath);
	bool Unserialize(const char* filePath);
};
