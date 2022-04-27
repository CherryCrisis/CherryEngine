#include "pch.hpp"

#include "engine.hpp"
#include "threadpool.hpp"

#include "scene_manager.hpp"
#include "csscripting_system.hpp"

#include "resource_manager.hpp"
#include "time_manager.hpp"
#include "input_manager.hpp"
#include "physic_manager.hpp"
#include "sound_manager.hpp"
#include "time_manager.hpp"
#include "cell_system.hpp"
#include "debug.hpp"

void* Engine::window_handle = nullptr;

Engine::Engine() 
{
	CsScriptingSystem::GetInstance()->Init();
}

Engine::~Engine()
{
	InputManager::Kill();
	PhysicSystem::PhysicManager::Kill();
	SoundManager::Kill();
	TimeManager::Kill();
	ResourceManager::Kill();
	SceneManager::Kill();
	CellSystem::Kill();
	CsScriptingSystem::Kill();
	Debug::Kill(); 
}

void Engine::TickEngine()
{
	ThreadPool::GetInstance()->Update(EChannelTask::MAINTHREAD); //TODO: stock threadpool in engine class
}

void Engine::Tick() 
{
	SceneManager::GetInstance()->Update();

	PhysicSystem::PhysicManager::GetInstance()->Simulate(1/60.f);
}

void Engine::Launch() 
{
	isPlaying = true;
	SceneManager::FlipScene();
	PhysicSystem::PhysicManager::GetInstance()->Launch();
}

void Engine::LaunchStandalone() 
{
	isPlaying = true;
	PhysicSystem::PhysicManager::GetInstance()->Launch();
}

void Engine::Stop()
{
	isPlaying = false;
	PhysicSystem::PhysicManager::GetInstance()->Stop();
	SceneManager::ResetScene();
}