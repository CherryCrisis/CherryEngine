#pragma once

#ifdef CHERRYDLL_EXPORTS
#define CHERRY_API __declspec(dllexport)
#else
#define CHERRY_API __declspec(dllimport)
#endif

#include <vector>
#include "Component.h"
#include "debug.hpp"

class CHERRY_API Engine 
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