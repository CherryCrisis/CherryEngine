#pragma once

#include <vector>

#include <cherry_macros.hpp>

#include "event.hpp"


class CCENGINE_API Engine
{
public:
	static void* window_handle;

	static bool isPlaying;
	static bool isPaused;
	
	static bool shouldStop;

	Event<> m_OnStop;
	
	Engine();
	~Engine();

	void Launch(bool flipScene = true);

	void Stop();

	void Tick();
	void TickEngine();

	void EndFrame();

	static void Quit() { shouldStop = true; }
};
