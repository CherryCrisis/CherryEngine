
#pragma once

#include "core/panel.hpp"

class ProjectSettingsDisplayer : public Panel
{
public:

	ProjectSettingsDisplayer(bool spawnOpened = true) : Panel(spawnOpened) {}

	void Render() override;

	// All the things + will have to serialize of all this shit
};