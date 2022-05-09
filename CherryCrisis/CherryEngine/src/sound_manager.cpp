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

}

SoundManager::~SoundManager()
{
    m_device = alcGetContextsDevice(m_context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}


bool SoundManager::Init() 
{
    // TODO: Send error messages
    SoundManager* instance = SoundManager::GetInstance();
    instance->m_device = alcOpenDevice(NULL);
    if (!instance->m_device) return false;

    instance->m_context = alcCreateContext(instance->m_device, NULL);
    if (!instance->m_context) return false;

    if (!alcMakeContextCurrent(instance->m_context)) return false;

    return true;
}