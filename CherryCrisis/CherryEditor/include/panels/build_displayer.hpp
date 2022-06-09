#pragma once

#include "core/panel.hpp"

class ProjectSettingsDisplayer;
class AssetBrowser;

class BuildDisplayer : public Panel 
{
public:
	std::string outDir = "OutDirectory/";

	ProjectSettingsDisplayer* projectSettings;
	AssetBrowser* browser = nullptr;

	BuildDisplayer(bool spawnOpened = true) : Panel(spawnOpened) {}

	void Render() override;

};