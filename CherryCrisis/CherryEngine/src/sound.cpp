#include <pch.hpp>

#include <sound.hpp>
#include "resource_manager.hpp"
#include "alut.h"

void Sound::Load(std::shared_ptr<Sound> sound)
{
	//Load Sound
	sound->m_buffer = alutCreateBufferFromFile(sound->GetFilepath().c_str());
	if (sound->m_buffer == AL_NONE)
	{
		unsigned int error = alutGetError();
		fprintf(stderr, "Error loading file: '%s'\n",
			alutGetErrorString(error));
		alutExit();
		exit(EXIT_FAILURE);
	}
	alGenSources(1, &(sound->m_source));
	alSourcei(sound->m_source, AL_BUFFER, sound->m_buffer);
}