#pragma once

#include "cherry_header.hpp"

class CCSCRIPT_API Printer : public Behaviour
{
public:
	Printer() : Behaviour() {}
	void Start();
	void Update();
};