#pragma once

#include <cherry_macros.hpp>

#include <vector>
#include "component.hpp"
#include "debug.hpp"

class CCENGINE_API Engine
{
public:
	Engine();
	std::vector<Behaviour*> behaviours;

	bool isPlaying = false;

	static void PlayFile(const char* fileNam);
	void Launch();
	void Stop() { isPlaying = false; behaviours.clear(); }
	void SoundInit();

	void Tick();
	void TickEngine();
};