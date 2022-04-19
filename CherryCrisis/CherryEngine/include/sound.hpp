#pragma once

#include "resource.hpp"
#include "cherry_macros.hpp"

class CCENGINE_API Sound : public Resource<Sound>
{
private:

	bool m_isLoop = false;

protected:

public:
	Sound(const char* filepath) : Resource(filepath) {}
	 
	unsigned int m_buffer = 0u;
	unsigned int m_source = 0u;
	
	void SetIsLoop(bool value) { m_isLoop = value; }
	bool IsLooping() { return m_isLoop; }

	float m_volumeMultiplier = 1.f;

	static void Load(std::shared_ptr<Sound> sound);

	void Delete() override {}
};