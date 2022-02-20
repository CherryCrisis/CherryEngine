#pragma once

#include <CherryMacros.h>

#include <vector>
#include "Component.h"
#include "debug.hpp"

class CCENGINE_API Engine
{
public:
	Engine();
	std::vector<Behaviour*> behaviours;

	bool isPlaying = false;

	static void PlayFile(const char* fileNam);
	void Launch();
	void Stop() { isPlaying = false; behaviours.clear(); Debug::GetInstance()->Clear(); }
	void SoundInit();

	void Tick();
};