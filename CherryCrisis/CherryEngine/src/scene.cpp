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

	for (const Entity& child : children)
		root.m_transform->AddChildren(child.m_transform);

	scene->m_entities.push_back(std::move(root));

	return scene;
}

void Scene::Draw()
{
	RenderManager::DrawScene();
}
