#include "pch.hpp"

#include "scene.hpp"

#include <iostream>

#include "resource_manager.hpp"
#include "render_manager.hpp"
#include "scene_manager.hpp"
#include "debug.hpp"

// TODO: Remove this
#include "entity.hpp"
#include "transform.hpp"
#include "model_renderer.hpp"

#include "model.hpp"
#include "mesh.hpp"

#include "serializer.hpp"
std::string Scene::m_defaultCellName = "Default";

Scene::Scene(const char* filePath)
	: Resource(filePath), m_debug(Debug::GetInstance()) 
{
}

void Scene::Delete()
{
	for (const Entity* entity : m_entities)
		delete entity;
}

Scene::~Scene()
{
	if (GetResourceState() != EResourceState::DESTROYED)
		Delete();
}

void Scene::Initialize()
{
	for (Entity* entity : m_entities)
		entity->Initialize();
}

void Scene::Update()
{
	for (Entity* entity : m_entities) 
	{
		entity->Update();
	}
}

void Scene::AddEntity(Entity* toAdd)
{
	m_entities.push_back(toAdd);
	m_onModifiedEntities.Invoke();
}

void Scene::RemoveEntity(Entity* toRemove) 
{
	if (!toRemove)
		return;

	Transform* transform;
	toRemove->TryGetBehaviour(transform);

	auto children = transform->GetChildren();
	if (transform && children->size() > 0 )
	{
		for (int i = 0; i < children->size(); i++)
			RemoveEntity(&(*children)[i]->GetHost());
	}

	m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), toRemove), m_entities.end());
	toRemove->Destroy();
	m_onModifiedEntities.Invoke();
}

void Scene::RemoveEntity(const std::string& name)
{
	//TODO: Do this
	m_onModifiedEntities.Invoke();
}

void Scene::Load(std::shared_ptr<Scene> scene)
{
	Serializer::UnserializeScene(scene);
}

bool Scene::Save() 
{
	return Serializer::SerializeScene(this);
}

bool Scene::SaveAs(const char* filepath)
{
	return Serializer::SerializeScene(this, filepath);
}

void Scene::GenerateEntitiesRecursive(ModelNode* node, Entity* parentEntity, std::vector<Entity*>& entities, Cell* cell)
{
	std::string entityName;

	if (!node->m_nodeName.empty())
		entityName = node->m_nodeName;
	else
		entityName = Entity::m_defaultName;

	Entity* entity = new Entity(entityName, cell);

	Transform* transform = entity->GetOrAddBehaviour<Transform>();

	ModelRenderer* modelRdr;

	if (node->m_mesh)
	{
		modelRdr = entity->AddBehaviour<ModelRenderer>();
		modelRdr->m_transform = transform;
		modelRdr->SetMesh(node->m_mesh);
		modelRdr->SetMaterial(node->m_material);
	}

	transform->SetPosition(node->m_baseTRS[0]);
	transform->SetRotation(node->m_baseTRS[1]);
	transform->SetScale(node->m_baseTRS[2]);

	if (parentEntity)
	{
		Transform* parentTransform = parentEntity->GetOrAddBehaviour<Transform>();
		transform->SetParent(parentTransform);
	}

	entities.push_back(entity);

	for (ModelNode* childNode : node->m_childrenNode)
		GenerateEntitiesRecursive(childNode, entity, entities, cell);
}

void Scene::GenerateEntities(std::shared_ptr<Model> modelBase)
{
	if (m_cells.size() == 0)
	{
		Debug::GetInstance()->AddLog(ELogType::ERROR, "Can't generate entities of modelBase, scene has no cell");
		return;
	}

	std::vector<Entity*> entities;

	ModelNode* rootNode = modelBase->GetRootNode();

	if (modelBase->GetMeshCount() && rootNode)
		GenerateEntitiesRecursive(rootNode, nullptr, entities, &m_cells.begin()->second);

	for (Entity* entity : entities)
	{
		AddEntity(entity);
		entity->Initialize();
	}
}

void Scene::GenerateEntitiesInCell(std::shared_ptr<Model> modelBase, Cell* cell)
{
	std::vector<Entity*> entities;

	ModelNode* rootNode = modelBase->GetRootNode();

	if (modelBase->GetMeshCount() && rootNode)
		GenerateEntitiesRecursive(rootNode, nullptr, entities, cell);

	for (Entity* entity : entities)
	{
		AddEntity(entity);
		entity->Initialize();
	}
}

Entity* Scene::FindEntity(uint32_t id)
{
	// TODO: Change this to unordered map of all uuid containers
	for (Entity* entity : m_entities)
	{
		if ((uint32_t)entity->GetUUID() == id)
			return entity;
	}

	return nullptr;
}

Entity* Scene::FindModelEntity(uint32_t id)
{
	for (Entity* entity : m_entities)
	{
		if (ModelRenderer* rdr = entity->GetBehaviour<ModelRenderer>(); rdr && rdr->m_id == id)
			return entity;
	}
	return nullptr;
}

void Scene::Empty() 
{
	while(m_entities.size() > 0)
	{
		m_entities[0]->Destroy();
		m_entities.erase(m_entities.begin());
	}

	while (m_cells.size() > 0)
	{
		for (auto& cell : m_cells)
		{
			RemoveCell(cell.first, true);
			break;
		}
	}
}

void Scene::EmptyUI() 
{
	while (m_UIContext.m_items.size() > 0)
	{
		delete m_UIContext.m_items[0];
		m_UIContext.m_items.erase(m_UIContext.m_items.begin());
	}
}

Cell* Scene::AddCell(const std::string& name, CCUUID uuid)
{
	Cell* cell = nullptr;
	// TODO: cache variables
	if (!m_cells.contains(name))
	{
		auto it = m_cells.insert({ name, { uuid } });
		cell = &it.first->second;

		cell->SetName(name);
		cell->Initialize();
	}

	return cell;
}

Cell* Scene::GetCell(const std::string& name)
{
	if (!m_cells.contains(name))
		return nullptr;

	return &m_cells[name];
}

bool Scene::RenameCell(const std::string& oldName, const std::string& newName)
{
	if (oldName == m_defaultCellName)
	{
		m_debug->AddLog(ELogType::WARNING, "Cannot rename default cell");
		return false;
	}

	if (m_cells.contains(oldName) && !m_cells.contains(newName))
	{
		auto cell = m_cells.extract(oldName);
		cell.key() = newName;
		m_cells.insert(move(cell));
		m_cells[newName].SetName(newName);

		return true;
	}
	return false;
}

bool Scene::RemoveCell(const std::string& name, bool forceRemove)
{
	if (forceRemove)
	{
		auto entities = m_cells[name].GetEntities();
		for (auto& entity : entities)
			RemoveEntity(entity);

		m_cells.erase(name);
		return true;
	}

	if (name == m_defaultCellName)
	{
		m_debug->AddLog(ELogType::WARNING, "Cannot remove default cell");
		return false;
	}

	if (m_cells.contains(name))
	{
		auto entities = m_cells[name].GetEntities();
		for (auto& entity : entities)
			MoveEntityFromCellToCell(name, m_defaultCellName, entity);

		m_cells.erase(name);
		return true;
	}
	return false;
}

void Scene::AddEntityToDefault(Entity* entity)
{
	m_cells[m_defaultCellName].AddEntity(entity);
}

void Scene::AddEntityToCell(Entity* entity, const std::string& cellName)
{
	assert(m_cells.contains(cellName));

	m_cells[cellName].AddEntity(entity);
}


void Scene::MoveEntityFromCellToCell(const std::string& fromCellName, const std::string& toCellName, Entity* entity)
{
	if (m_cells.contains(fromCellName) && m_cells.contains(toCellName))
		MoveEntityFromCellToCell(&m_cells[fromCellName], &m_cells[toCellName], entity);
}

void Scene::MoveEntityFromCellToCell(Cell* fromCell, Cell* toCell, Entity* entity)
{
	if (!m_cells.contains(fromCell->GetName()) || !m_cells.contains(toCell->GetName()))
		return;

	int i = fromCell->PossessEntity(entity);
	if (i == -1)
	{
		m_debug->AddLog(ELogType::ERROR, "Trying to remove an entity from a cell not owning it");
		return;
	}

	i = toCell->PossessEntity(entity);
	if (i != -1)
	{
		m_debug->AddLog(ELogType::ERROR, "Trying to move an entity to a cell already owning it");
		return;
	}

	fromCell->RemoveEntity(entity);
	entity->m_OnCellRemoved.Invoke(&*fromCell);

	toCell->AddEntity(entity);
	entity->m_OnCellAdded.Invoke(&*toCell);
}
