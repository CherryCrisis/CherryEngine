#include <pch.hpp>

#include "al.h"
#include "audio_listener.hpp"
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
	if (m_transform)
	{
		m_transform->m_onPositionChange.Unbind(&AudioListener::ChangePosition, this);
		m_transform->m_onRotationChange.Unbind(&AudioListener::ChangeRotation, this);
		m_transform->m_OnDestroy.Unbind(&AudioListener::InvalidateTransform, this);
	}
}

void AudioListener::Initialize()
{
	m_transform = GetHost().GetOrAddBehaviour<Transform>();

	if (m_transform)
	{
		m_transform->m_onPositionChange.Bind(&AudioListener::ChangePosition, this);
		m_transform->m_onRotationChange.Bind(&AudioListener::ChangeRotation, this);

		m_transform->m_OnDestroy.Bind(&AudioListener::InvalidateTransform, this);
	}

	ChangePosition(m_transform);
	ChangeRotation(m_transform);

	GetHost().m_OnAwake.Unbind(&AudioListener::Initialize, this);
}

void AudioListener::BindToSignals()
{
	GetHost().m_OnAwake.Bind(&AudioListener::Initialize, this);
}

void AudioListener::UnbindSignals() 
{

}

void AudioListener::ChangePosition(Transform* transform)
{
	Vector3 pos = transform->GetPosition();
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
}

void AudioListener::ChangeRotation(Transform* transform)
{
	if (!transform)return;

	CCMaths::Vector3 up  = transform->GetWorldMatrix().up;
	CCMaths::Vector3 fwd = transform->GetWorldMatrix().back;
	ALfloat listenerOri[] = 
	{ 
		fwd.x,fwd.y,fwd.z,
		up.x,up.y,up.z 
	};

	alListenerfv(AL_ORIENTATION, listenerOri);
}

void AudioListener::InvalidateTransform() 
{
	m_transform = nullptr;
}