#include "pch.hpp"

#include "scene_manager.hpp"
#include "resource_manager.hpp"
#include "serializer.hpp"

#include "utils.hpp"

template <>
SceneManager* Singleton<SceneManager>::currentInstance = nullptr;

void SceneManager::SetCurrentScene(std::shared_ptr<Scene> scene)
{
	m_currentScene = scene;
	Initialize();
}

// TODO: Change this
void SceneManager::Initialize()
{
	if (m_currentScene)
		m_currentScene->Initialize();
}

void SceneManager::Update()
{
	if (m_currentScene)
		m_currentScene->Update();
}

bool SceneManager::LoadScene(const char* filepath) 
{
	SceneManager* mng = GetInstance();

	mng->m_currentScene =  ResourceManager::GetInstance()->AddResource<Scene>(filepath, false);
	mng->Initialize();

	return mng->m_currentScene != nullptr;
}

bool SceneManager::LoadEmptyScene(const char* filepath) 
{
	CopyFolder("Internal/SampleScene.cherry", "Assets/Empty.cherry");
	return LoadScene("Assets/Empty.cherry");
}


bool SceneManager::SaveCurrentScene() 
{
	return GetInstance()->m_currentScene->Save();
}

void SceneManager::ResetScene() 
{
	SceneManager* mng = GetInstance();
	Serializer::UnserializeScene(mng->m_currentScene, "Internal/temp");
}

void SceneManager::FlipScene() 
{
	SceneManager* mng = GetInstance();
	mng->m_currentScene->SaveAs("Internal/temp");
}