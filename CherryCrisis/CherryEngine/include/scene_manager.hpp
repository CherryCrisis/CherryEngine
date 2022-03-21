#pragma once

#include <cherry_macros.hpp>
#include "singleton.hpp"

#include "scene.hpp"

class CCENGINE_API SceneManager : public Singleton<SceneManager>
{
	friend class Singleton<SceneManager>;

	std::shared_ptr<Scene> m_currentScene;

public:
	void SetCurrentScene(std::shared_ptr<Scene> scene);

	void Start();
	void Update();
};