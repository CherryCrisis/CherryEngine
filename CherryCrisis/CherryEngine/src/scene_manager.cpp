#include "pch.hpp"

#include "scene_manager.hpp"

template <>
SceneManager* Singleton<SceneManager>::currentInstance = nullptr;

void SceneManager::SetCurrentScene(std::shared_ptr<Scene> scene)
{
	m_currentScene = scene;
}

// TODO: Change this
void SceneManager::Start()
{
	if (m_currentScene)
		m_currentScene->Start();
}

void SceneManager::Update()
{
	if (m_currentScene)
		m_currentScene->Update();
}
