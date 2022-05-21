#pragma once

#include <cherry_macros.hpp>

#include "behaviour.hpp"
#include "maths/vector3.hpp"
#include "renderer.hpp"
#include "sound.hpp"

class Transform;


class CCENGINE_API AudioEmitter : public Behaviour, public ARenderer
{
private:
	using boolProperty	= CCProperty::ConstRefProperty<AudioEmitter, bool>;
	using floatProperty	= CCProperty::ConstRefProperty<AudioEmitter, float>;

	bool  m_isAutoplaying	  = true;
	bool  m_isLooping		  = false;
	bool  m_isSpatial		  = false;
	float m_pitch			  = 1.f;
	float m_rollOff           = 1.f;
	float m_referenceDistance = 1.f;

	std::string				m_soundPath = "null";
	std::shared_ptr<Sound>	m_sound;

	void PopulateMetadatas() override;

public:
	Transform* m_transform = nullptr;

	AudioEmitter();
	AudioEmitter(CCUUID& id);
	~AudioEmitter();

	void Play();
	void Pause();
	void Stop();
	
	void SetLooping(const bool& loop);
	void SetPitch(const float& value);
	void SetRollOff(const float& value);
	void SetReferenceDistance(const float& value);
	void SetSpatialized(const bool& value);

	//Load the sound if not existing, and adds it as the selected sound
	void AddSound(const std::string& filepath);
	void AddSound(std::shared_ptr<Sound> sound);

	void Initialize();
	void Start();
	void BindToSignals()	 override;

	void ChangePosition(const CCMaths::Vector3& position);
	void ChangeRotation(const CCMaths::Vector3& position);
	
	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;

	bool	    GetLooping()			{ return m_isLooping; }
	bool	    GetSpatialized()		{ return m_isSpatial; }
	float	    GetRollOff()			{ return m_rollOff; }

	float	    GetReferenceDistance()	{ return m_referenceDistance; }
	float	    GetPitch()				{ return m_pitch;     }
	std::string GetSoundPath()			{ return m_soundPath; }

	boolProperty	Looping{ this, &AudioEmitter::SetLooping,   &AudioEmitter::GetLooping };
	boolProperty	Spatialized{ this, &AudioEmitter::SetSpatialized,   &AudioEmitter::GetSpatialized };
	floatProperty	Pitch{ this, &AudioEmitter::SetPitch,   &AudioEmitter::GetPitch };
	floatProperty	ReferenceDistance{ this, &AudioEmitter::SetReferenceDistance,   &AudioEmitter::GetReferenceDistance};
	floatProperty	RollOff{ this, &AudioEmitter::SetRollOff,   &AudioEmitter::GetRollOff};

	CCProperty::ConstRefProperty<AudioEmitter, std::string> SoundPath{ this, &AudioEmitter::AddSound, &AudioEmitter::GetSoundPath };
};