#include <pch.hpp>

#include "shape_renderer.hpp"

#include <string>

#include "render_manager.hpp"
#include "resource_manager.hpp"

#include "cell.hpp"
#include "entity.hpp"
#include "material.hpp"
#include "model.hpp"
#include "texture.hpp"
#include "transform.hpp"


ShapeRenderer::ShapeRenderer() 
	: MeshRenderer(ERendererType::SHAPE)
{
	PopulateMetadatas();
}

ShapeRenderer::ShapeRenderer(CCUUID& id) 
	: MeshRenderer(ERendererType::SHAPE, id)
{
	PopulateMetadatas();
}

ShapeRenderer::~ShapeRenderer()
{
	RemoveMesh();
	RemoveMaterial();

	GetHost().m_cell->RemoveRenderer(this);

	GetHost().m_OnCellAdded.Unbind(&ShapeRenderer::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Unbind(&ShapeRenderer::OnCellRemoved, this);
}

void ShapeRenderer::PopulateMetadatas()
{
	m_metadatas.SetProperty("Type", &m_MeshType);
}

void ShapeRenderer::SetType(const int& type)
{
	m_meshType = static_cast<EMeshShape>(type);
}

int ShapeRenderer::GetType()
{
	return static_cast<int>(m_meshType);
}

void ShapeRenderer::OnMeshLoaded(std::shared_ptr<Mesh> mesh)
{
	mesh->m_OnLoaded.Unbind(&ShapeRenderer::OnMeshLoaded, this);
	SetMesh(mesh);
}

void ShapeRenderer::Initialize()
{
	m_initialized = true;

	switch (m_meshType)
	{
	case EMeshShape::QUAD:
		SetMesh(ResourceManager::GetInstance()->AddResource<Mesh>("CC_NormalizedPlane", true, EMeshShape::QUAD, 1.f, 1.f));
		break;
	case EMeshShape::CUBE:
		SetMesh(ResourceManager::GetInstance()->AddResource<Mesh>("CC_NormalizedCube", true, EMeshShape::CUBE, 1.f, 1.f, 1.f));
		break;
	case EMeshShape::SPHERE:
		SetMesh(ResourceManager::GetInstance()->AddResource<Mesh>("CC_NormalizedSphere", true, EMeshShape::SPHERE, 1.f, 17.f, 33.f));
		break;
	case EMeshShape::CYLINDER:
		SetMesh(ResourceManager::GetInstance()->AddResource<Mesh>("CC_NormalizedCylinder", true, EMeshShape::CYLINDER, 1.f, 1.f, 32.f));
		break;
	default:
		break;
	}
	
	if (m_mesh)
		GetHost().m_cell->AddRenderer(this);

	GetHost().m_OnAwake.Unbind(&ShapeRenderer::Initialize, this);
}

void ShapeRenderer::BindToSignals()
{
	GetHost().m_OnAwake.Bind(&ShapeRenderer::Initialize, this);
	GetHost().m_OnCellAdded.Bind(&ShapeRenderer::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Bind(&ShapeRenderer::OnCellRemoved, this);
}

void ShapeRenderer::OnCellAdded(Cell* newCell)
{
	if (m_mesh)
		newCell->AddRenderer(this);
}

void ShapeRenderer::OnCellRemoved(Cell* newCell)
{
	if (m_mesh)
		newCell->RemoveRenderer(this);
}
