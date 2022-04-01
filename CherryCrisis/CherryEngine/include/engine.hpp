#pragma once

#include <cherry_macros.hpp>

#include <vector>
#include "debug.hpp"

class CCENGINE_API Engine
{
public:

	static void* window_handle;

	Engine();

	bool isPlaying = false;

	void Launch();
	void Stop();

	void Tick();
	void TickEngine();
};