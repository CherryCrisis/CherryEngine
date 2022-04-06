#include "pch.hpp"

#include "scene_manager.hpp"

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
