#pragma once

#include <cherry_macros.hpp>

#include <vector>

#include "event.hpp"

class CCENGINE_API Engine
{
public:

	static void* window_handle;

	Engine();
	~Engine();

	bool m_isPlaying = false;
	bool m_isPaused = false;

	void Launch(bool flipScene = true);

	void Stop();

	void Tick();
	void TickEngine();

	Event<> m_OnStop;
};
