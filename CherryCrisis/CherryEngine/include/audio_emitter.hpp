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
	const char* m_soundPath = "null";

	bool  m_isAutoplaying = true;
	bool  m_isLooping     = false;
	bool  m_isSpatial     = false;
	float m_pitch		  = 1.f;
	
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
	void SetSpatialized(const bool& value);

	//Load the sound if not existing, and adds it as the selected sound
	void AddSound(const char* filepath);
	void AddSound(std::shared_ptr<Sound> sound);

	Transform* m_transform = nullptr;

	void Initialize();
	void Start();
	void BindToSignals()	 override;

	void ChangePosition(const CCMaths::Vector3& position);
	void ChangeRotation(const CCMaths::Vector3& position);

	bool	    GetLooping()	 { return m_isLooping; }
	bool	    GetSpatialized() { return m_isSpatial; }
	float	    GetPitch()       { return m_pitch;     }
	const char* GetSoundPath()   { return m_soundPath; }

	boolProperty	Looping{ this, &AudioEmitter::SetLooping,   &AudioEmitter::GetLooping };
	boolProperty	Spatialized{ this, &AudioEmitter::SetSpatialized,   &AudioEmitter::GetSpatialized };
	floatProperty	Pitch{ this, &AudioEmitter::SetPitch,   &AudioEmitter::GetPitch };

	CCProperty::CopyProperty<AudioEmitter, const char*> SoundPath{ this, &AudioEmitter::AddSound, &AudioEmitter::GetSoundPath };
};