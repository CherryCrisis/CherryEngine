#include "pch.hpp"

#include "engine.hpp"
#include "threadpool.hpp"

#include "scene_manager.hpp"
#include "csscripting_system.hpp"
#include "time_manager.hpp"

#include "input_manager.hpp"
#include "physic_manager.hpp"

#include "physic_scene.hpp"
#include "physic_actor.hpp"

void* Engine::window_handle = nullptr;

Engine::Engine() 
{
	CsScriptingSystem::GetInstance()->Init();
}

void Engine::TickEngine()
{
	ThreadPool::GetInstance()->Update(EChannelTask::MAINTHREAD); //TODO: stock threadpool in engine class
}

void Engine::Tick() 
{
	SceneManager::GetInstance()->Update();

	PhysicSystem::PhysicManager::GetInstance()->Simulate(1/60.f);

	if (InputManager::GetInstance()->GetKeyDown(Keycode::ESCAPE))
	{
		InputManager::GetInstance()->SetCursorDisplayed();
	}
}

void Engine::Launch() 
{
	isPlaying = true;

	SceneManager::GetInstance()->Start();

	InputManager::GetInstance()->SetContext("user Context");
	InputManager::GetInstance()->SetCursorHidden();
	PhysicSystem::PhysicManager::GetInstance()->Launch();
}

void Engine::Stop()
{
	isPlaying = false;
	
	PhysicSystem::PhysicManager::GetInstance()->Stop();
}