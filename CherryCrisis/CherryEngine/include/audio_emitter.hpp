#pragma once

#include "behaviour.hpp"

#include "cherry_macros.hpp"
#include "sound.hpp"
#include "maths/vector3.hpp"

class Transform;

class CCENGINE_API AudioEmitter : public Behaviour
{
private:
	std::shared_ptr<Sound> m_sound;
public:
	AudioEmitter();
	AudioEmitter(CCUUID& id);
	~AudioEmitter();

	void Play();
	void Pause();
	void Stop();

	void SetLooping(bool loop);
	void SetPitch(float value);
	
	//Load the sound if not existing, and adds it as the selected sound
	void AddSound(const char* filepath);

	Transform* m_transform = nullptr;

	void Initialize();
	void BindToSignals() override;

	void ChangePosition(const CCMaths::Vector3& position);
	void ChangeRotation(const CCMaths::Vector3& position);
};