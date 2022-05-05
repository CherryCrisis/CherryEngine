#include <pch.hpp>

#include "audio_emitter.hpp"

#include "resource_manager.hpp"
#include "transform.hpp"

void AudioEmitter::Play()
{
	if (!m_sound) return;
	m_sound->Play();
}
void AudioEmitter::Pause() 
{
	if (!m_sound) return;
	m_sound->Pause();
}

void AudioEmitter::Stop() 
{
	if (!m_sound) return;
	m_sound->Stop();
}

void AudioEmitter::SetLooping(bool loop) 
{
	if (!m_sound) return;
	m_sound->SetLooping(loop);
}
void AudioEmitter::SetPitch(float value) 
{
	if (!m_sound) return;
	m_sound->SetPitch(value);
}

//Load the sound if not existing, and adds it as the selected sound
void AudioEmitter::AddSound(const char* filepath)
{
	m_sound = ResourceManager::GetInstance()->AddResource<Sound>(filepath, true);
}

AudioEmitter::AudioEmitter()
{
	PopulateMetadatas();
}

AudioEmitter::AudioEmitter(CCUUID& id) : Behaviour(id)
{
	PopulateMetadatas();
}

AudioEmitter::~AudioEmitter()
{

}

void AudioEmitter::Initialize()
{
	m_transform = GetHost().GetOrAddBehaviour<Transform>();

	if (m_transform)
	{
		m_transform->m_onPositionChange.Bind(&AudioEmitter::ChangePosition, this);
		m_transform->m_onRotationChange.Bind(&AudioEmitter::ChangeRotation, this);
	}

	ChangePosition(m_transform->GetPosition());
	ChangeRotation(m_transform->GetRotation());

	GetHost().m_OnAwake.Unbind(&AudioEmitter::Initialize, this);
}

void AudioEmitter::BindToSignals()
{
	GetHost().m_OnAwake.Bind(&AudioEmitter::Initialize, this);
}

void AudioEmitter::ChangePosition(const CCMaths::Vector3& position)
{
	if (!m_sound)return;
	m_sound->SetPosition(position);
}

void AudioEmitter::ChangeRotation(const CCMaths::Vector3& rotation)
{
	// TODO: Fix this, need forward and up as param
	//alListener3f(AL_ORIENTATION, position.x, position.y, position.z);
}