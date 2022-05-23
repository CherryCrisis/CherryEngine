#include <pch.hpp>

#include "audio_emitter.hpp"

#include "resource_manager.hpp"

#include "entity.hpp"
#include "transform.hpp"
#include "guizmo_renderpass.hpp"
#include "camera_component.hpp"

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

void AudioEmitter::SetLooping(const bool& loop) 
{
	if (!m_sound) return;
	m_isLooping = loop;
	m_sound->SetLooping(m_isLooping);
}
void AudioEmitter::SetPitch(const float& value) 
{
	if (!m_sound) return;
	m_pitch = value;
	m_sound->SetPitch(m_pitch);
}

void AudioEmitter::SetRollOff(const float& value)
{
	if (!m_sound) return;
	m_rollOff = value;
	m_sound->SetRollOff(m_rollOff);
}

void AudioEmitter::SetReferenceDistance(const float& value)
{
	if (!m_sound) return;
	m_referenceDistance = value;
	m_sound->SetReferenceDistance(m_referenceDistance);
}
void AudioEmitter::SetSpatialized(const bool& value)
{
	if (!m_sound) return;
	m_isSpatial = value;
	if (m_isSpatial && m_transform) 
	{
		ChangePosition(m_transform->GetPosition());
		ChangeRotation(m_transform->GetRotation());
	}
}

//Load the sound if not existing, and adds it as the selected sound
void AudioEmitter::AddSound(const std::string& filepath)
{
	m_sound = ResourceManager::GetInstance()->AddResource<Sound>(filepath.c_str(), true);
	m_soundPath = m_sound->m_filepath.string();
}

void AudioEmitter::AddSound(std::shared_ptr<Sound> sound)
{
	m_sound = sound;
	m_soundPath = m_sound->m_filepath.string();
}

AudioEmitter::AudioEmitter()
{
	PopulateMetadatas();

	if (CameraComponent::m_editorCamera)
		SubscribeToPipeline(CameraComponent::m_editorCamera->m_pipeline.get());
}

AudioEmitter::AudioEmitter(CCUUID& id) : Behaviour(id)
{
	PopulateMetadatas();

	if (CameraComponent::m_editorCamera)
		SubscribeToPipeline(CameraComponent::m_editorCamera->m_pipeline.get());
}

AudioEmitter::~AudioEmitter()
{
	if (CameraComponent::m_editorCamera)
		UnsubscribeToPipeline(CameraComponent::m_editorCamera->m_pipeline.get());

	m_transform->m_onPositionChange.Unbind(&AudioEmitter::ChangePosition, this);
	m_transform->m_onRotationChange.Unbind(&AudioEmitter::ChangeRotation, this);
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
	GetHost().m_OnStart.Bind(&AudioEmitter::Start, this);
	GetHost().m_OnDestroyed.Bind(&AudioEmitter::Stop, this);
	
}

void AudioEmitter::ChangePosition(const CCMaths::Vector3& position)
{
	if (!m_sound || !m_isSpatial) return;
	m_sound->SetPosition(position);
}

void AudioEmitter::ChangeRotation(const CCMaths::Vector3& rotation)
{
	if (!m_sound || !m_isSpatial) return;
	return;
	m_sound->SetDirection(rotation);
}

void AudioEmitter::PopulateMetadatas() 
{
	Behaviour::PopulateMetadatas();

	// change all of this to properties
	m_metadatas.SetProperty("Sound", &SoundPath, "dropzone");
	m_metadatas.SetField("AutoPlay", m_isAutoplaying);
	m_metadatas.SetProperty("Looping", &Looping);
	m_metadatas.SetProperty("Spatialized", &Spatialized);
	m_metadatas.SetProperty("Pitch", &Pitch);
	m_metadatas.SetProperty("RollOff", &RollOff);
	m_metadatas.SetProperty("ReferenceDistance", &ReferenceDistance);
}

void AudioEmitter::Start() 
{
	if (m_isAutoplaying)
		Play();

	GetHost().m_OnStart.Unbind(&AudioEmitter::Start, this);
}

void AudioEmitter::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->SubscribeToPipeline<GuizmoRenderPass>(this);
}

void AudioEmitter::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->UnsubscribeToPipeline<GuizmoRenderPass>(this);
}

void AudioEmitter::Copy(Behaviour* copy) 
{
	AudioEmitter* emitterCopy = dynamic_cast<AudioEmitter*>(copy);
	
	AddSound(emitterCopy->GetSoundPath());
	
	m_isAutoplaying = emitterCopy->m_isAutoplaying;
	m_isLooping = emitterCopy->m_isLooping;
	m_isSpatial = emitterCopy->m_isSpatial;
	m_pitch = emitterCopy->m_pitch;
	m_rollOff = emitterCopy->m_rollOff;
	m_referenceDistance = emitterCopy->m_referenceDistance;

	m_transform = GetHost().GetBehaviour<Transform>();

	Initialize();
}