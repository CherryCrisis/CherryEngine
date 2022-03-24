#pragma once

#include <cherry_macros.hpp>

#include <vector>
#include "debug.hpp"

class CCENGINE_API Engine
{
public:
	Engine();

	bool isPlaying = false;

	void Launch();
	void Stop();

	void Tick();
	void TickEngine();
};