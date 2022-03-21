#pragma once

#include "cherry_macros.hpp"
#include "singleton.hpp"
#include "maths/vector3.hpp"

class CCENGINE_API SoundManager : public Singleton<SoundManager> 
{
public:

	void PlaySound2D(unsigned int id, const float volume = 1.f);
	void PlaySoundAt(const CCMaths::Vector3& position = CCMaths::Vector3::Zero, const float volume = 1.f);

	SoundManager();
};