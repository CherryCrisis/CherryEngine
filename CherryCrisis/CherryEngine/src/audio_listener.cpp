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
	if (!m_transform)return;

	CCMaths::Vector3 up  = m_transform->GetWorldMatrix().up;
	CCMaths::Vector3 fwd = m_transform->GetWorldMatrix().back;
	ALfloat listenerOri[] = 
	{ 
		fwd.x,fwd.y,fwd.z,
		up.x,up.y,up.z 
	};

	alListenerfv(AL_ORIENTATION, listenerOri);
}