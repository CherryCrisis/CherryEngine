#pragma once

#include <cherry_macros.hpp>
#include "singleton.hpp"

#include "scene.hpp"

class CCENGINE_API SceneManager : public Singleton<SceneManager>
{
	friend class Singleton<SceneManager>;

	Resource::Ref<Scene> m_currentScene;

public:
	void SetCurrentScene(Resource::Ref<Scene> scene);

	void Start();
	void Update();
};