#pragma once

#include <string>
#include <vector>

#include "resource.hpp"
#include "entity.hpp"

class ModelBase;
struct ModelNode;

class CCENGINE_API Scene : public Resource<Scene>
{ 
public:
	
	Scene(const char* filePath) : Resource(filePath) { }
	virtual ~Scene();
	
	std::vector<Entity*> m_entities;

	//Call entities Initialize 
	void Initialize();
	
	//Call entities Update
	void Update();

	static void Load(std::shared_ptr<Scene> scene);

	void PopulateEmpty();

	
	void AddEntity(Entity* toAdd);
	void RemoveEntity(Entity* toRemove);
	void RemoveEntity(const std::string& name);

	//Find Entity by its UUID
	Entity* FindEntity(uint32_t id);
	//!!INTERNAL!! - Find Entity by its modelID (picking)
	Entity* FindModelEntity(uint32_t id);

	void GenerateEntities(std::shared_ptr<ModelBase> resource);
	void GenerateEntitiesRecursive(ModelNode* node, Entity* parentEntity, std::vector<Entity*>& entities);

	std::string GetName() { return GetFilepath(); }

	bool Save();
	bool SaveAs(const char* filepath);

	//Clear the entity vector
	void Empty();

	void Delete() override;
};
