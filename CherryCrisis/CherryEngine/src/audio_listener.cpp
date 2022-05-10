#include <pch.hpp>

#include "audio_listener.hpp"

#include "al.h"

#include "entity.hpp"
#include "transform.hpp"

AudioListener::AudioListener()
{
	PopulateMetadatas();
}

AudioListener::AudioListener(CCUUID& id) : Behaviour(id)
{
	PopulateMetadatas();
}

AudioListener::~AudioListener()
{

}

void AudioListener::Initialize()
{
	m_transform = GetHost().GetOrAddBehaviour<Transform>();

	if (m_transform)
	{
		m_transform->m_onPositionChange.Bind(&AudioListener::ChangePosition, this);
		m_transform->m_onRotationChange.Bind(&AudioListener::ChangeRotation, this);
	}

	ChangePosition(m_transform->GetPosition());
	ChangeRotation(m_transform->GetRotation());

	GetHost().m_OnAwake.Unbind(&AudioListener::Initialize, this);
}

void AudioListener::BindToSignals()
{
	GetHost().m_OnAwake.Bind(&AudioListener::Initialize, this);
}

void AudioListener::ChangePosition(const CCMaths::Vector3& position)
{
	alListener3f(AL_POSITION, position.x, position.y, position.z);
}

void AudioListener::ChangeRotation(const CCMaths::Vector3& rotation)
{
	// TODO: Fix this, need forward and up as param
	//alListener3f(AL_ORIENTATION, position.x, position.y, position.z);
}