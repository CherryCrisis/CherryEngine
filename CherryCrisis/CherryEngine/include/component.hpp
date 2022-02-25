#pragma once

#include <cherry_macros.hpp>

class CCENGINE_API Behaviour
{
public:

	Behaviour() {}
	virtual void Start() {};
	virtual void Update() {};
};