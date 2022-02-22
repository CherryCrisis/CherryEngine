#include "scene.hpp"

#include "resourceManager.hpp"
#include "render_manager.hpp"

#include "modelBase.hpp"

Scene* Scene::Create(const char* filePath)
{
	Scene* scene = new Scene(filePath);

	auto RM = ResourceManager::GetInstance();

	std::shared_ptr<ModelBase> modelBase = RM->AddResource<ModelBase>("../Assets/backpack.obj", true);

	Entity root;
	const std::vector<Entity>& children = modelBase->GenerateEntities(root);
	scene->m_entities.push_back(std::move(root));

	for (const Entity& child : children)
	{
		scene->m_entities.push_back(std::move(child));
		root.m_transform->AddChildren(child.m_transform);
	}

	Entity light;
	light.m_light = new LightComponent();
	scene->m_entities.push_back(std::move(light));



	return scene;
}

void Scene::Draw()
{
	RenderManager::DrawScene();
}
