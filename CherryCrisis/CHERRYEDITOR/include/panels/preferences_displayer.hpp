
#include "core/panel.hpp"

class PreferencesDisplayer : public Panel
{
public:
	
	PreferencesDisplayer(bool spawnOpened = true) : Panel(spawnOpened) {}

	void Render() override;


	// All the things + will have to serialize of all this shit
};