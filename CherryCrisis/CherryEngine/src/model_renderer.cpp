#include <pch.hpp>

#include "model_renderer.hpp"

#include <string>

#include "render_manager.hpp"
#include "resource_manager.hpp"

#include "cell.hpp"
#include "entity.hpp"
#include "material.hpp"
#include "model.hpp"
#include "texture.hpp"


ModelRenderer::ModelRenderer() 
	: MeshRenderer(ERendererType::MODEL)
{
	PopulateMetadatas();
}

ModelRenderer::ModelRenderer(CCUUID& id) 
	: MeshRenderer(ERendererType::MODEL, id)
{
	PopulateMetadatas();
}

ModelRenderer::~ModelRenderer()
{
	RemoveMesh();
	RemoveMaterial();

	GetHost().m_OnCellAdded.Unbind(&ModelRenderer::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Unbind(&ModelRenderer::OnCellRemoved, this);
}

void ModelRenderer::PopulateMetadatas()
{
	m_metadatas.SetProperty("meshFile", &m_MeshPath, "dropzone off");
}

void ModelRenderer::SetMeshFromPath(std::string meshPath)
{
	if (!meshPath.empty())
		SetMesh(ResourceManager::GetInstance()->AddResourceRef<Mesh>(meshPath.c_str()));
}

std::string ModelRenderer::GetMeshPath()
{
	return m_mesh ? m_mesh->GetFilepath() : "";
}

void ModelRenderer::LoadModelFromPath(std::string modelPath)
{
	if (!modelPath.empty())
		ResourceManager::GetInstance()->AddResourceMultiThreads<Model>(modelPath.c_str(), true, nullptr);
}

std::string ModelRenderer::GetModelPath()
{
	std::string path(m_mesh->GetFilepath());
	return m_mesh ? path.c_str() : "";
}

void ModelRenderer::OnMeshLoaded(std::shared_ptr<Mesh> mesh)
{
	mesh->m_OnLoaded.Unbind(&ModelRenderer::OnMeshLoaded, this);
	SetMesh(mesh);
}

void ModelRenderer::Initialize() 
{
	m_initialized = true;

	if (m_mesh)
		GetHost().m_cell->AddRenderer(this);

	GetHost().m_OnAwake.Unbind(&ModelRenderer::Initialize, this);
}

void ModelRenderer::BindToSignals() 
{
	GetHost().m_OnAwake.Bind(&ModelRenderer::Initialize, this);
	GetHost().m_OnCellAdded.Bind(&ModelRenderer::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Bind(&ModelRenderer::OnCellRemoved, this);
}

void ModelRenderer::OnCellAdded(Cell* newCell)
{
	if (m_mesh)
		newCell->AddRenderer(this);
}

void ModelRenderer::OnCellRemoved(Cell* newCell)
{
	if (m_mesh)
		newCell->RemoveRenderer(this);
}
