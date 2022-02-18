#include "scene.hpp"

#include "resourceManager.hpp"
#include "render_manager.hpp"

#include "modelBase.hpp"

Scene* Scene::Create(const char* filePath)
{
	Scene* scene = new Scene(filePath);

	auto RM = ResourceManager::GetInstance();

	std::shared_ptr<ModelBase> modelBase = RM->AddResource<ModelBase>("../Assets/weapon.obj", true);

	Entity root;
	modelBase->GenerateEntities(root);

	scene->m_entities.push_back(root);

	return scene;
}

void Scene::Draw()
{
	RenderManager::DrawScene();
}
