#pragma once 

#include <cherry_macros.hpp>

class CCENGINE_API Builder 
{
public:
	static bool BuildGame(const char* outDir = "", const char* gameName="MyCherryGame", bool runGame = false);
};