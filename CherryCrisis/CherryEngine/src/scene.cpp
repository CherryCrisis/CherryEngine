#include "pch.hpp"

#include "scene.hpp"

#include <iostream>

#include "resource_manager.hpp"
#include "render_manager.hpp"

// TODO: Remove this
#include "transform.hpp"
#include "light_component.hpp"
#include "model_renderer.hpp"
#include "camera_component.hpp"
#include "scripted_behaviour.hpp"

#include "model_base.hpp"
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
		entity->Initialize(this);
}

void Scene::Update()
{
 	for (Entity* entity : m_entities)
		entity->Update();
}

void Scene::PopulateEmpty() 
{
	AddOrGetCell(m_defaultCellName);

	Entity* light = new Entity("Light");
	light->AddBehaviour<LightComponent>(); 	
	AddEntity(light);

	Entity* camera = new Entity("Camera");
	camera->AddBehaviour<Transform>();
	auto cameraComp = camera->AddBehaviour<CameraComponent>();
	cameraComp->BindToSignals();

	ScriptedBehaviour* bhave1 = camera->AddBehaviour<ScriptedBehaviour>();
	bhave1->SetScriptClass("CameraController"); 
	bhave1->BindToSignals();
	AddEntity(camera);
}

void Scene::AddEntity(Entity* toAdd)
{
	m_entities.push_back(toAdd);
}

void Scene::RemoveEntity(Entity* toRemove) 
{
	if (!toRemove)
		return;

	Transform* transform;
	toRemove->TryGetBehaviour(transform);
	if (transform && transform->GetChildren().size() > 0 )
	{
		std::vector<Transform*> children = transform->GetChildren();

		for (int i = 0; i < children.size(); i++)
			RemoveEntity(&children[i]->GetHost());
	}

	m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), toRemove), m_entities.end());
	toRemove->Destroy();
}

void Scene::RemoveEntity(const std::string& name)
{
	//TODO: Do this
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

void Scene::GenerateEntitiesRecursive(ModelNode* node, Entity* parentEntity, std::vector<Entity*>& entities)
{
	Entity* entity = new Entity();


	if (!node->m_nodeName.empty())
		entity->SetName(node->m_nodeName);

	Transform* transform = entity->GetOrAddBehaviour<Transform>();

	ModelRenderer* modelRdr;

	if (node->m_mesh)
	{
		modelRdr = entity->AddBehaviour<ModelRenderer>();
		modelRdr->m_transform = transform;
		modelRdr->SetMesh(node->m_mesh);
		modelRdr->SetMaterial(node->m_material);
		modelRdr->BindToSignals();

	}

	transform->SetPosition(node->m_baseTRS[0]);
	transform->SetRotation(node->m_baseTRS[1]);
	transform->SetScale(node->m_baseTRS[2]);

	transform->BindToSignals();

	if (parentEntity)
	{
		Transform* parentTransform = parentEntity->GetOrAddBehaviour<Transform>();
		transform->SetParent(parentTransform);
	}

	entities.push_back(entity);

	for (ModelNode* childNode : node->m_childrenNode)
		GenerateEntitiesRecursive(childNode, entity, entities);
}

void Scene::GenerateEntities(std::shared_ptr<ModelBase> modelBase)
{
	std::vector<Entity*> entities;

	ModelNode* rootNode = modelBase->GetRootNode();

	if (modelBase->GetMeshCount() && rootNode)
		GenerateEntitiesRecursive(rootNode, nullptr, entities);

	for (Entity* entity : entities)
	{
		AddEntity(entity);
		entity->Initialize(this);
	}
}

void Scene::GenerateEntitiesInCell(std::shared_ptr<ModelBase> modelBase, const std::string& cellName)
{
	std::vector<Entity*> entities;

	ModelNode* rootNode = modelBase->GetRootNode();

	if (modelBase->GetMeshCount() && rootNode)
		GenerateEntitiesRecursive(rootNode, nullptr, entities);

	for (Entity* entity : entities)
	{
		AddEntity(entity);
		entity->Initialize(this, cellName);
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
}

Cell* Scene::AddOrGetCell(const std::string& name)
{
	// TODO: cache variables
	if (!m_cells.contains(name))
	{
		m_cells[name];

		m_cells[name].SetName(name);
		m_cells[name].Initialize();
	}

	return &m_cells[name];
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

bool Scene::RemoveCell(const std::string& name)
{
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