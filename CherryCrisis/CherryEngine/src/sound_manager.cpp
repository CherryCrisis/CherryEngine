#include <pch.hpp>

#include "sound_manager.hpp"

#include "al.h"
#include "alc.h"


template <>
SoundManager* Singleton<SoundManager>::currentInstance = nullptr;

SoundManager::SoundManager() 
{

}

SoundManager::~SoundManager()
{
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}

void SoundManager::PlaySound2D(unsigned int id, const float volume)
{
	// General play sound
	alSourcePlay(id);
}

void SoundManager::PlaySoundAt(const CCMaths::Vector3& position, const float volume)
{
	// Play Sound at a given location nsm
}

bool SoundManager::Init() 
{
    // TODO: Send error messages
    SoundManager* instance = SoundManager::GetInstance();
    instance->m_device = alcOpenDevice(nullptr);

    if (!instance->m_device) return false;

    instance->m_context = alcCreateContext(instance->m_device, NULL);
    if (!instance->m_context) return false;

    if (!alcMakeContextCurrent(instance->m_context)) return false;

    return true;
}