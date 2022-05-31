#include <pch.hpp>

#include "scene.hpp"

#include <iostream>

#include "debug.hpp"
#include "render_manager.hpp"
#include "resource_manager.hpp"
#include "scene_manager.hpp"

#include "entity.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "model_renderer.hpp"
#include "serializer.hpp"
#include "shape_renderer.hpp"
#include "transform.hpp"


std::string Scene::m_defaultCellName = "Default";

Scene::Scene(const char* filePath)
	: Resource(filePath), m_debug(Debug::GetInstance()) 
{
}

void Scene::Delete()
{
	while (!m_entities.empty())
		RemoveEntity(m_entities[0].get());
}

Scene::~Scene()
{
	if (GetResourceState() != EResourceState::DESTROYED)
		Delete();
}

void Scene::Initialize()
{
	for (auto& entity : m_entities)
		entity->Initialize();
}

void Scene::Update()
{
	size_t entitiesCount = m_entities.size();
	for (size_t id = 0; id < entitiesCount; id++)
	{
		//To prevent if an entity is deleted in scene->Update() !
		if (entitiesCount != m_entities.size())
			break;

		m_entities[id]->Update();
	}
}

void Scene::FixedUpdate()
{
	size_t entitiesCount = m_entities.size();
	for (size_t id = 0; id < entitiesCount; id++)
	{
		//To prevent if an entity is deleted in scene->Update() !
		if (entitiesCount != m_entities.size())
			break;

		m_entities[id]->FixedUpdate();
	}
}

void Scene::LateUpdate()
{
	size_t entitiesCount = m_entities.size();
	for (size_t id = 0; id < entitiesCount; id++)
	{
		//To prevent if an entity is deleted in scene->Update() !
		if (entitiesCount != m_entities.size())
			break;

		m_entities[id]->LateUpdate();
	}
}

Entity* Scene::AddEntity(std::unique_ptr<Entity>& toAdd)
{
	assert(toAdd != nullptr);

	m_isHierarchyDirty = true;

	m_entities.push_back(std::move(toAdd));
	
	return m_entities[m_entities.size() - 1].get();

}

void Scene::RemoveEntity(Entity* toRemove)
{
	if (!toRemove)
		return;
	
	Transform* transform;
	toRemove->TryGetBehaviour(transform);

	if (transform)
	{
		auto children = transform->GetChildren();
		if (transform && children->size() > 0)
		{
			for (int i = static_cast<int>(children->size()) - 1; i >= 0; i--)
				RemoveEntity(&(*children)[i]->GetHost());
		}
	}

	int entitiesCount = m_entities.size();
	for (int id = 0; id < entitiesCount; ++id)
	{
		if (m_entities[id].get() == toRemove)
		{
			m_entities[id].reset(nullptr);
			m_entities[id] = std::move(m_entities[entitiesCount - 1]);
			m_entities.pop_back();
			break;
		}
	}

	//toRemove->Destroy();
	m_isHierarchyDirty = true;
}

void Scene::RemoveEntity(const std::string& name)
{
	//TODO: Do this
	m_isHierarchyDirty = true;
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

void Scene::GenerateEntitiesRecursive(ModelNode* node, Entity* parentEntity, std::vector<std::unique_ptr<Entity>>& entities, Cell* cell)
{
	std::string entityName;

	if (!node->m_nodeName.empty())
		entityName = node->m_nodeName;
	else
		entityName = Entity::m_defaultName;

	std::unique_ptr<Entity> entity = std::make_unique<Entity>(entityName, cell);

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

	entities.push_back(std::move(entity));

	for (ModelNode* childNode : node->m_childrenNode)
		GenerateEntitiesRecursive(childNode, entity.get(), entities, cell);
}

void Scene::GenerateEntities(std::shared_ptr<Model> modelBase)
{
	if (m_cells.size() == 0)
	{
		Debug::GetInstance()->AddLog(ELogType::ERROR, "Can't generate entities of modelBase, scene has no cell");
		return;
	}

	std::vector<std::unique_ptr<Entity>> entities;

	ModelNode* rootNode = modelBase->GetRootNode();

	if (modelBase->GetMeshCount() && rootNode)
		GenerateEntitiesRecursive(rootNode, nullptr, entities, &m_cells.begin()->second);

	for (auto& entity : entities)
	{
		AddEntity(entity);
		entity->Initialize();
	}
}

void Scene::GenerateEntitiesInCell(std::shared_ptr<Model> modelBase, Cell* cell)
{
	std::vector<std::unique_ptr<Entity>> entities;

	ModelNode* rootNode = modelBase->GetRootNode();

	if (modelBase->GetMeshCount() && rootNode)
		GenerateEntitiesRecursive(rootNode, nullptr, entities, cell);

	for (auto& entity : entities)
	{
		AddEntity(entity);
		entity->Initialize();
	}
}

Entity* Scene::FindEntity(uint32_t id)
{
	// TODO: Change this to unordered map of all uuid containers
	for (auto& entity : m_entities)
	{
		if ((uint32_t)entity->GetUUID() == id)
			return entity.get();
	}

	return nullptr;
}

Entity* Scene::FindModelEntity(uint32_t id)
{
	for (auto& entity : m_entities)
	{
		if (ModelRenderer* rdr = entity->GetBehaviour<ModelRenderer>(); rdr && rdr->m_id == id)
			return entity.get();

		else if (ShapeRenderer* rdr = entity->GetBehaviour<ShapeRenderer>(); rdr && rdr->m_id == id)
			return entity.get();
	}
	return nullptr;
}

void Scene::Empty() 
{
	Delete();

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
	m_isHierarchyDirty = true;
}

void Scene::AddEntityToCell(Entity* entity, const std::string& cellName)
{
	assert(m_cells.contains(cellName));

	m_cells[cellName].AddEntity(entity);
	m_isHierarchyDirty = true;
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
		m_debug->AddLog(ELogType::ERROR, std::format("Trying to remove an entity ({}) from a cell ({}) not owning it", entity->GetName(),fromCell->GetName()).c_str());
		return;
	}

	i = toCell->PossessEntity(entity);
	if (i != -1)
	{
		m_debug->AddLog(ELogType::ERROR, std::format("Trying to move an entity ({}) to a cell ({}) already owning it", entity->GetName(), fromCell->GetName()).c_str());
		return;
	}

	fromCell->RemoveEntity(entity);
	entity->m_OnCellRemoved.Invoke(&*fromCell);

	toCell->AddEntity(entity);
	entity->m_OnCellAdded.Invoke(&*toCell);

	//Recursive call
	if (Transform* transform = entity->GetBehaviour<Transform>())
	{
		auto childrens = *transform->GetChildren();
		for (Transform* child : childrens)
			MoveEntityFromCellToCell(fromCell, toCell, child->GetHostPtr());
	}

	m_isHierarchyDirty = true;
}

void Scene::CopyEntity(Entity* toCopy, Entity* parent) 
{
	std::unique_ptr<Entity> entity = std::make_unique<Entity>(toCopy);
	
	if (parent)
		entity->GetBehaviour<Transform>()->SetParent(parent->GetBehaviour<Transform>());

	AddEntity(entity);
}
