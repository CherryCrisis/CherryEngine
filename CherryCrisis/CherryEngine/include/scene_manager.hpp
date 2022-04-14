#pragma once

#include <cherry_macros.hpp>
#include "singleton.hpp"
#include "scene.hpp"

class CCENGINE_API SceneManager : public Singleton<SceneManager>
{
	friend class Singleton<SceneManager>;

public:
	void SetCurrentScene(std::shared_ptr<Scene> scene);

	std::shared_ptr<Scene> m_currentScene;
	std::shared_ptr<Scene> m_scenePreset;

	void Initialize();
	void Update();

	// Load a scene and set is as the current scene
	static bool LoadScene(const char* filepath);
	// Save the current scene into its file
	static bool SaveCurrentScene();

	static void ResetScene();
	static void FlipScene();
};