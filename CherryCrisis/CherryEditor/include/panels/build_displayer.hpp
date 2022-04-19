#pragma once

#include "core/panel.hpp"

class ProjectSettingsDisplayer;

class BuildDisplayer : public Panel 
{
public:
	ProjectSettingsDisplayer* projectSettings;

	BuildDisplayer(bool spawnOpened = true) : Panel(spawnOpened) {}

	void Render() override;

	char outDir[128] = "idk/idk/idk/idk";
};