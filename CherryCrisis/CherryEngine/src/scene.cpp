#include "pch.hpp"

#include "scene.hpp"

#include "resource_manager.hpp"
#include "render_manager.hpp"
#include "callback.hpp"

#include "model_base.hpp"

void Scene::GenerateEntities(std::shared_ptr<ModelBase> resource)
{
	Entity root;
	const std::vector<Entity>& children = resource->GenerateEntities(root);
	m_entities.push_back(std::move(root));

	for (const Entity& child : children)
	{
		m_entities.push_back(std::move(child));
		root.m_transform->AddChildren(child.m_transform);
	}

	std::cout << "entities generated : " << resource->GetFilepath() << std::endl;
}

void Scene::Load(std::shared_ptr<Scene> scene)
{
	auto RM = ResourceManager::GetInstance();

	auto callback = CCCallback::BindCallback(&Scene::GenerateEntities, scene.get());
	RM->AddResourceMultiThreads<ModelBase>("../assets/Backpack-1/backpack.obj", true, callback);

	Entity& light = scene->m_entities.emplace_back();
	light.m_lightComp = new LightComponent();

	Entity& camera = scene->m_entities.emplace_back();
	camera.m_cameraComp = new CameraComponent();
	camera.m_cameraComp->m_transform = camera.m_transform;
}

void Scene::Draw()
{
	RenderManager::DrawScene();
}
