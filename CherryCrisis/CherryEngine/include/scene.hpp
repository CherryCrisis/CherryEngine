#pragma once

#include <map>
#include <string>
#include <vector>

#include "cell.hpp"
#include "resource.hpp"
#include "ui_context.hpp"

struct ModelNode;
class Debug;
class Entity;
class Model;


class CCENGINE_API Scene : public Resource<Scene>
{ 
private:
	Debug* m_debug = nullptr;

	static std::string m_defaultCellName;

public:
	UIContext m_UIContext;

	std::vector<Entity*> m_entities;
	std::map<std::string, Cell> m_cells;

	Event<> m_onModifiedEntities;
	
	Scene(const char* filePath);
	virtual ~Scene();

	Cell*	AddCell(const std::string& name, CCUUID uuid = {});
	Cell*	GetCell(const std::string& name);
	bool	RenameCell(const std::string& oldName, const std::string& newName);
	bool	RemoveCell(const std::string& name, bool forceRemove = false);

	void	AddEntityToDefault(Entity* entity);
	void	AddEntityToCell(Entity* entity, const std::string& cellName);
	void	MoveEntityFromCellToCell(const std::string& fromCell, const std::string& toCell, Entity* entity);
	void	MoveEntityFromCellToCell(Cell* fromCell, Cell* toCell, Entity* entity);


	//Call entities Initialize 
	void Initialize();
	
	//Call entities Update
	void Update();

	static void Load(std::shared_ptr<Scene> scene);
	
	void AddEntity(Entity* toAdd);
	void RemoveEntity(Entity* toRemove);
	void RemoveEntity(const std::string& name);

	//Find Entity by its UUID
	Entity* FindEntity(uint32_t id);
	//!!INTERNAL!! - Find Entity by its modelID (picking)
	Entity* FindModelEntity(uint32_t id);

	void GenerateEntities(std::shared_ptr<Model> resource);
	void GenerateEntitiesInCell(std::shared_ptr<Model> resource, Cell* cell);
	void GenerateEntitiesRecursive(ModelNode* node, Entity* parentEntity, std::vector<Entity*>& entities, Cell* cell);

	std::string GetName() { return GetFilepath(); }

	bool Save();
	bool SaveAs(const char* filepath);

	//Clear the entity vector
	void Empty();
	void EmptyUI();
	void Delete() override;
};
