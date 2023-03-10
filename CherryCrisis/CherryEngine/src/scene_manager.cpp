#include <pch.hpp>

#include "scene_manager.hpp"

#include "debug.hpp"
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

void SceneManager::FixedUpdate()
{
	if (m_currentScene)
		m_currentScene->FixedUpdate();
}

void SceneManager::LateUpdate()
{
	if (m_currentScene)
		m_currentScene->LateUpdate();

	if (m_lateChanged)
		m_lateChanged->Invoke();
}

void SceneManager::SetHierarchyDirty(bool value) 
{
	if (GetInstance() && GetInstance()->m_currentScene)
		currentInstance->m_currentScene->m_isHierarchyDirty = value;
}

bool SceneManager::LoadScene(const char* filepath) 
{
	float preTime = 0.f;
	if (Engine::isPlaying)
		preTime = TimeManager::GetInstance()->GetTime();
	
	SceneManager* mng = GetInstance();
	
	if (!std::filesystem::exists(filepath)) 
	{
		std::string errorMsg = filepath + std::string(" failed to load.");
		Debug::GetInstance()->AddLog(ELogType::WARNING, errorMsg.c_str());
		return LoadEmptyScene("Assets/EmptyScene.ccscene");
	}	

	EmptyScene();

	mng->m_currentScene =  ResourceManager::GetInstance()->AddResource<Scene>(filepath, false);
	
	if (Engine::isPlaying)
		TimeManager::GetInstance()->SetTime(preTime);

	mng->Initialize();

	ResourceManager::GetInstance()->Purge();

	mng->m_sceneChanged.Invoke();

	return mng->m_currentScene != nullptr;
}

bool SceneManager::LoadEmptyScene(const char* filepath) 
{
	CopyFile("Internal/SampleScene.ccscene", filepath);
	return LoadScene("Assets/Empty.ccscene");
}


bool SceneManager::SaveCurrentScene() 
{
	if (Engine::isPlaying || Engine::isPaused)
		return false;

	return GetInstance()->m_currentScene->Save();
}

void SceneManager::ResetScene() 
{
	SceneManager* mng = GetInstance();
	
	EmptyScene();

	mng->m_currentScene = ResourceManager::GetInstance()->AddResource<Scene>(mng->m_baseScene.c_str(), true);
	Serializer::UnserializeScene(mng->m_currentScene, "Internal/temp");
	mng->Initialize();

	ResourceManager::GetInstance()->Purge();

	mng->m_sceneChanged.Invoke();
}

void SceneManager::EmptyScene() 
{
	SceneManager* mng = GetInstance();

	if (mng->m_currentScene)
		mng->m_currentScene->Empty();
}

void SceneManager::FlipScene() 
{
	SceneManager* mng = GetInstance();
	mng->m_currentScene->SaveAs("Internal/temp");
	mng->m_baseScene = mng->m_currentScene->GetFilepath();
}

// filepath starts at Assets/
bool SceneManager::ChangeScene(const char* sceneName)
{
	std::string fullPath = "Assets/" + std::string(sceneName) + ".ccscene";

	if (Engine::isPlaying || Engine::isPaused) 
	{
		currentInstance->m_lateChanged = CCFunction::BindFunction(&SceneManager::LateLoadScene, 
			currentInstance, std::forward<std::string>(fullPath));

		return false;
	}

	return LoadScene(fullPath.c_str());
}

void SceneManager::LateLoadScene(std::string filepath) 
{
	LoadScene(filepath.c_str());
	m_lateChanged.reset();
}