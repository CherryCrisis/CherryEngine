#include <pch.hpp>

#include "engine.hpp"

#include "csscripting_system.hpp"
#include "debug.hpp"
#include "scene_manager.hpp"
#include "threadpool.hpp"

#include "input_manager.hpp"
#include "physic_manager.hpp"
#include "pickinger.hpp"
#include "render_manager.hpp"
#include "resource_manager.hpp"
#include "sound_manager.hpp"
#include "time_manager.hpp"


void* Engine::window_handle  = nullptr;
bool Engine::isPaused        = false;
bool Engine::isPlaying	     = false;
bool Engine::shouldStop      = false;

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
	SceneManager::GetInstance()->m_currentScene.reset();
	ResourceManager::Kill();
	InputManager::Kill();
 	SoundManager::Kill();
	TimeManager::Kill();
	SceneManager::Kill();
	CsScriptingSystem::Kill();
	Pickinger::Kill();
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

	int fixedUpdates = TimeManager::GetFixedLoopCount();
	for (int i = 0; i < fixedUpdates; i++)
	{
		SceneManager::GetInstance()->FixedUpdate();
		PhysicSystem::PhysicManager::GetInstance()->Simulate();
	}

	SceneManager::GetInstance()->LateUpdate();
}

void Engine::Launch(bool flipScene) 
{
	isPlaying = true;
	
	if (flipScene)
		SceneManager::FlipScene();
	
	PhysicSystem::PhysicManager::GetInstance()->Launch();
}

void Engine::Stop()
{
	isPlaying = false;
	isPaused = false;
	PhysicSystem::PhysicManager::GetInstance()->Stop();
	SceneManager::ResetScene();
	m_OnStop.Invoke();
}

void Engine::EndFrame() 
{
	if (shouldStop)
		Stop();

	shouldStop = false;
}