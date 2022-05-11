
#include "core/panel.hpp"
#include <array>
#include <string>

class PreferencesDisplayer : public Panel
{
private:
	std::array<PanelCategory*, 2> m_categories;

	int m_selectedCategory = 0;

	//-------------Nested Classes

	class Cutstomization : public PanelCategory
	{
	public:
		Cutstomization(std::string name = "default") : PanelCategory(name) {}
		void Fill() override;
	};

	class ThirdParty : public PanelCategory
	{
	public:
		ThirdParty(std::string name = "default") : PanelCategory(name) {}
		void Fill() override;

		char m_compilerPath[256] = "C:\\Program Files(x86)\\Microsoft Visual Studio\\2019\\Community\\Common7\\IDE\\devenv";
	};

public:
	
	PreferencesDisplayer(bool spawnOpened);// : Panel(spawnOpened) {}

	void Render() override;

	std::string   GetCompilerPath();

	// All the things + will have to serialize of all this shit
};