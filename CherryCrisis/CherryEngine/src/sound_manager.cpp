#include <pch.hpp>

#include <sound_manager.hpp>

#include "al.h"
#include "alc.h"
#include "alut.h"

template <>
SoundManager* Singleton<SoundManager>::currentInstance = nullptr;

void SoundManager::PlaySound2D(unsigned int id, const float volume)
{
	// General play sound
	alSourcePlay(id);
}

void SoundManager::PlaySoundAt(const CCMaths::Vector3& position, const float volume)
{
	// Play Sound at a given location nsm
}

SoundManager::SoundManager() 
{
	// Initialize OpenAL
	if (!alutInit(nullptr, nullptr))
	{
		ALenum error = alutGetError();
		fprintf(stderr, "%s\n", alutGetErrorString(error));
	}
}