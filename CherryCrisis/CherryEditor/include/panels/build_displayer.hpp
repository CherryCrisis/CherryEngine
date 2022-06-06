#pragma once

#include "core/panel.hpp"

class ProjectSettingsDisplayer;
class AssetBrowser;

class BuildDisplayer : public Panel 
{
public:
	char outDir[128] = "OutDirectory/";

	ProjectSettingsDisplayer* projectSettings;
	AssetBrowser* browser = nullptr;

	BuildDisplayer(bool spawnOpened = true) : Panel(spawnOpened) {}

	void Render() override;

};