#pragma once

#include "core/panel.hpp"

class ProjectSettingsDisplayer;


class BuildDisplayer : public Panel 
{
public:
	char outDir[128] = "OutDirectory/";

	ProjectSettingsDisplayer* projectSettings;

	BuildDisplayer(bool spawnOpened = true) : Panel(spawnOpened) {}

	void Render() override;
};