#pragma once

#include <cherry_macros.hpp>

#include <vector>

class CCENGINE_API Engine
{
public:

	static void* window_handle;

	Engine();
	~Engine();

	bool isPlaying = false;

	void Launch();
	void Stop();

	void Tick();
	void TickEngine();
};