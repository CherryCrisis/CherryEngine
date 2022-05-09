#pragma once

#include "behaviour.hpp"

#include "cherry_macros.hpp"
#include "sound.hpp"
#include "maths/vector3.hpp"

class Transform;

class CCENGINE_API AudioEmitter : public Behaviour
{
private:
	using boolProperty = CCProperty::ConstRefProperty<AudioEmitter, bool>;
	using floatProperty = CCProperty::ConstRefProperty<AudioEmitter, float>;

	std::shared_ptr<Sound> m_sound;
	std::string m_soundPath = "null";

	bool  m_isAutoplaying	  = true;
	bool  m_isLooping		  = false;
	bool  m_isSpatial		  = false;
	float m_pitch			  = 1.f;
	float m_rollOff           = 1.f;
	float m_referenceDistance = 1.f;

	void PopulateMetadatas() override;
public:
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

	Transform* m_transform = nullptr;

	void Initialize();
	void Start();
	void BindToSignals()	 override;

	void ChangePosition(const CCMaths::Vector3& position);
	void ChangeRotation(const CCMaths::Vector3& position);

	bool	    GetLooping()		   { return m_isLooping; }
	bool	    GetSpatialized()	   { return m_isSpatial; }
	float	    GetRollOff() { return m_rollOff; }

	float	    GetReferenceDistance() { return m_referenceDistance; }
	float	    GetPitch()			   { return m_pitch;     }
	std::string GetSoundPath()		   { return m_soundPath; }

	boolProperty	Looping{ this, &AudioEmitter::SetLooping,   &AudioEmitter::GetLooping };
	boolProperty	Spatialized{ this, &AudioEmitter::SetSpatialized,   &AudioEmitter::GetSpatialized };
	floatProperty	Pitch{ this, &AudioEmitter::SetPitch,   &AudioEmitter::GetPitch };

	//float data[10240];
	floatProperty	RollOff{ this, &AudioEmitter::SetRollOff,   &AudioEmitter::GetRollOff};
	floatProperty	ReferenceDistance{ this, &AudioEmitter::SetReferenceDistance,   &AudioEmitter::GetReferenceDistance};
	
	CCProperty::ConstRefProperty<AudioEmitter, std::string> SoundPath{ this, &AudioEmitter::AddSound, &AudioEmitter::GetSoundPath };
};