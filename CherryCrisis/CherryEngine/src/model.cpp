#include "pch.hpp"

#include "model.hpp"

#include "resource_manager.hpp"
#include "mesh.hpp"
#include "material.hpp"

void Model::Load(std::shared_ptr<Model> model, std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
	model->m_mesh = mesh;
	model->m_material = material;
}

void Model::Load(std::shared_ptr<Model> model, std::shared_ptr<Mesh> mesh) 
{
	model->m_mesh = mesh;
}

void Model::Delete()
{
	m_mesh = nullptr;
	m_material = nullptr;
}

void Model::Reload(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
	m_mesh = mesh;
	m_material = material;
}
