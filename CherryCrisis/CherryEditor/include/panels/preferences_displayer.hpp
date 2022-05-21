#pragma once

#include <array>
#include <string>

#include "core/panel.hpp"


class PreferencesDisplayer : public Panel
{
private:
	class Cutstomization : public PanelCategory
	{
	public:
		Cutstomization(std::string name = "default") : PanelCategory(name) {}
		void Fill() override;
	};

	class ThirdParty : public PanelCategory
	{
	public:
		char m_compilerPath[256] = "C:\\Program Files(x86)\\Microsoft Visual Studio\\2019\\Community\\Common7\\IDE\\devenv";
		
		ThirdParty(std::string name = "default") : PanelCategory(name) {}
		void Fill() override;
	};

	int m_selectedCategory = 0;

	std::array<PanelCategory*, 2> m_categories;

public:
	PreferencesDisplayer(bool spawnOpened);

	void Render() override;

	std::string   GetCompilerPath();
};