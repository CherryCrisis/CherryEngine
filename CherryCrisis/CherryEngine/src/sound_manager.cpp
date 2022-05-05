#include <pch.hpp>

#include <sound_manager.hpp>

#include "al.h"
#include "alc.h"

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
    // TODO: Send error messages
    ALCdevice* device = alcOpenDevice(NULL);
    if (!device) return;

    ALCcontext* context = alcCreateContext(device, NULL);
    if (!context) return;

    if (!alcMakeContextCurrent(context)) return;
}