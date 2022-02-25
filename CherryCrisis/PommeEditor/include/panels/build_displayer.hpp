#pragma once

#include "core/panel.hpp"

class BuildDisplayer : public Panel 
{
public:

	BuildDisplayer(bool spawnOpened = true) : Panel(spawnOpened) {}

	void Render() override;
};