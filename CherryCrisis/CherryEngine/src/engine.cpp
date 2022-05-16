#include "pch.hpp"

#include "engine.hpp"
#include "threadpool.hpp"

#include "scene_manager.hpp"
#include "csscripting_system.hpp"

#include "resource_manager.hpp"
#include "render_manager.hpp"
#include "time_manager.hpp"
#include "input_manager.hpp"
#include "physic_manager.hpp"
#include "sound_manager.hpp"
#include "time_manager.hpp"
#include "cell_system.hpp"
#include "debug.hpp"
#include "pickinger.hpp"

void* Engine::window_handle = nullptr;

Engine::Engine() 
{
	ThreadPool::CreateInstance();
	InputManager::CreateInstance();
	PhysicSystem::PhysicManager::CreateInstance();
	SoundManager::CreateInstance();
	TimeManager::CreateInstance();
	Debug::CreateInstance();
	ResourceManager::CreateInstance();
	CsScriptingSystem::CreateInstance();
	RenderManager::CreateInstance();
	Pickinger::CreateInstance();
	SceneManager::CreateInstance();

	CsScriptingSystem::GetInstance()->Init();
	SoundManager::Init();
	InputManager::GetInstance()->Init();
}

Engine::~Engine()
{
	PhysicSystem::PhysicManager::Kill();
	InputManager::Kill();
	SoundManager::Kill();
	TimeManager::Kill();
	ResourceManager::Kill();
	SceneManager::Kill();
	CellSystem::Kill();
	CsScriptingSystem::Kill();
	RenderManager::Kill();
	Debug::Kill();
	ThreadPool::Kill();
}

void Engine::TickEngine()
{
	ThreadPool::GetInstance()->Update(EChannelTask::MAINTHREAD);
}

void Engine::Tick() 
{
	SceneManager::GetInstance()->Update();

	PhysicSystem::PhysicManager::GetInstance()->Simulate(1/60.f);
}

void Engine::Launch(bool flipScene) 
{
	m_isPlaying = true;
	
	if (flipScene)
		SceneManager::FlipScene();
	
	PhysicSystem::PhysicManager::GetInstance()->Launch();
}

void Engine::Stop()
{
	m_isPlaying = false;
	m_isPaused = false;
	PhysicSystem::PhysicManager::GetInstance()->Stop();
	SceneManager::ResetScene();
	m_OnStop.Invoke();
}
