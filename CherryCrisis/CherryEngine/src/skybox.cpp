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

void Skybox::Load()
{
	m_mesh = ResourceManager::GetInstance()->AddResource<Mesh>("CC_normalizedCube", true, EMeshShape::CUBE, 0.5f, 0.5f, 0.5f);

	// TODO: Remove this
	const char* textures[6] = { "../assets/right.jpg", "../assets/left.jpg", "../assets/top.jpg", "../assets/bottom.jpg", "../assets/front.jpg", "../assets/back.jpg" };

	m_cubemap = ResourceManager::GetInstance()->AddResource<Cubemap>("skyCubemap", true, textures);

	RenderManager::instance()->GenerateFromPipeline<SkyboxRenderPass>(this);
}