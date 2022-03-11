#pragma once

#include <cherry_macros.hpp>

#include <vector>
#include "debug.hpp"

class CCENGINE_API Engine
{
public:
	Engine();

	bool isPlaying = false;

	static void PlayFile(const char* fileNam);
	void Launch();
	void Stop() { isPlaying = false; }
	void SoundInit();

	void Tick();
};