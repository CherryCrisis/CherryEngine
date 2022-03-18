#include "pch.hpp"

#include "engine.hpp"
#include "threadpool.hpp"

#include "scene_manager.hpp"
#include "csscripting_system.hpp"

Engine::Engine() 
{
	CsScriptingSystem::GetInstance()->Init(mono::ManagedScriptSystemSettings_t("RootDomain"));
}

void Engine::TickEngine()
{
	ThreadPool::GetInstance()->Update(EChannelTask::MAINTHREAD); //TODO: stock threadpool in engine class
}

void Engine::Tick() 
{
	SceneManager::GetInstance()->Update();
}

void Engine::Launch() 
{
	isPlaying = true;

	SceneManager::GetInstance()->Start();
}
