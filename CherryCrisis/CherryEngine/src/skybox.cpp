#include "pch.hpp"

#include "skybox.hpp"

#include "resource_manager.hpp"

#include "cubemap.hpp"
#include "mesh.hpp"

#include "skybox_renderpass.hpp"
#include "render_manager.hpp"
#include "threadpool.hpp"

Skybox::Skybox()
{
	auto function = CCFunction::BindFunction(&Skybox::Load, this);
	ThreadPool::GetInstance()->CreateTask(function, EChannelTask::MAINTHREAD);
}

Skybox::~Skybox()
{
	if (m_mesh)
		m_cubemap->m_OnDeleted.Unbind(&Skybox::RemoveMesh, this);

	if (m_mesh)
		m_cubemap->m_OnDeleted.Unbind(&Skybox::RemoveMesh, this);
}

void Skybox::RemoveMesh()
{
	m_mesh = nullptr;
	UnsubscribeToRenderPass();
}

void Skybox::RemoveCubemap()
{
	m_mesh = nullptr;
	UnsubscribeToRenderPass();
}

void Skybox::Load()
{
	m_mesh = ResourceManager::GetInstance()->AddResource<Mesh>("CC_normalizedCube", true, EMeshShape::CUBE, 0.5f, 0.5f, 0.5f);
	m_mesh->m_OnDeleted.Bind(&Skybox::RemoveMesh, this);

	// TODO: Remove this
	const char* textures[6] = { "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg" };

	m_cubemap = ResourceManager::GetInstance()->AddResource<Cubemap>("skyCubemap", true, textures, "Assets/skybox");
	m_cubemap->m_OnDeleted.Bind(&Skybox::RemoveCubemap, this);

	SubscribeToRenderPass();
}

void Skybox::SubscribeToRenderPass()
{
	RenderManager::GetInstance()->GenerateFromPipeline<SkyboxRenderPass>(this);
}

void Skybox::UnsubscribeToRenderPass()
{
	RenderManager::GetInstance()->RemoveFromPipeline<SkyboxRenderPass>(this);
}
