
#pragma once

#include "core/panel.hpp"
#include <array>
#include <string>

class ProjectSettingsDisplayer : public Panel
{
public:
	ProjectSettingsDisplayer(bool spawnOpened);

	void Render() override;

	// All the things + will have to serialize of all this shit
	
	std::array<std::string, 6> m_categories;
};