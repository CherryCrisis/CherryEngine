#pragma once

#include <cherry_macros.hpp>

#include "singleton.hpp"

#include "function.hpp"
#include "scene.hpp"


class CCENGINE_API SceneManager : public Singleton<SceneManager>
{
	friend class Singleton<SceneManager>;

private:
	std::string m_baseScene = "";

	void LateLoadScene(std::string filepath);

public:
	std::shared_ptr<Scene> m_currentScene;

	std::unique_ptr<CCFunction::AFunction> m_lateChanged;

	Event<> m_lateHierarchyRefresh;
	Event<> m_sceneChanged; // Useful for editor callbacks
	void SetCurrentScene(std::shared_ptr<Scene> scene);

	void Initialize();
	void Update();
	void FixedUpdate();
	void LateUpdate();

	static void SetHierarchyDirty(bool value = true);
	// Load a scene and set is as the current scene
	static bool LoadScene(const char* filepath);
	// Save the current scene into its file
	static bool SaveCurrentScene();
	
	static bool LoadEmptyScene(const char* filepath);

	// C# interfacing function : Only Name required 
	static bool ChangeScene(const char* sceneName);

	static void ResetScene();
	static void FlipScene();
	static void EmptyScene();
};
