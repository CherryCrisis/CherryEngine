#include "pch.hpp"

#include "scene.hpp"

#include "resource_manager.hpp"
#include "render_manager.hpp"
#include "callback.hpp"

#include "model_base.hpp"

void Scene::GenerateEntitiesTest(std::shared_ptr<ModelBase> resource)
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

	auto RM = ResourceManager::GetInstance();
	auto callback = CCCallback::BindCallback(&Scene::GenerateEntitiesTest, this);
	RM->AddResourceMultiThreads<ModelBase>("../assets/Backpack-1/backpack.obj", true, callback);
}

void Test(std::shared_ptr<int> i)
{
	int* iPtr = i.get();
	*iPtr += 5;
	std::cout << std::format("int i : {}", *i.get()) << std::endl;
}

void Scene::Load(Ref<Scene> scene, const char* filePath)
{

	//{
	//	//EXEMPLE N1
	//	auto callback3 = CCCallback::BindCallback(Test);
	//	auto wrapcallback = static_cast<CCCallback::AWrapCallback*>(callback3.release());
	//	std::shared_ptr<int> copyShrInt = scene->m_int;
	//	auto function = CCFunction::BindFunction(&CCCallback::AWrapCallback::Invoke, 
	//		wrapcallback, scene->m_int);
	//	ThreadPool::GetInstance()->CreateTask(function, EChannelTask::MAINTHREAD);

	//	//EXEMPLE N2
	//	auto callback4 = CCCallback::BindCallback(Test);
	//	scene->m_eventCallback.Bind(callback4);
	//	std::shared_ptr<int> copyShrInt2 = scene->m_int;

	//	scene->m_eventCallback.Invoke(std::move(copyShrInt2));
	//}

	auto RM = ResourceManager::GetInstance();

	auto callback = CCCallback::BindCallback(&Scene::GenerateEntities, scene.get());
	RM->AddResourceMultiThreads<ModelBase>("../assets/Backpack-1/backpack.obj", true, callback);

	//auto callback2 = CCCallback::BindCallback(&Scene::GenerateEntities, scene.get());
	//RM->AddResourceMultiThreads<ModelBase>("../assets/Backpack-1/backpack.obj", true, callback2);

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
