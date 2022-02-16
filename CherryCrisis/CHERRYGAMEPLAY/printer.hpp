#pragma once

#include <CherryHeader.h>

class CHERRY_API Printer : public Behaviour
{
public:
	Printer() : Behaviour() {}
	void Start();
	void Update();
};