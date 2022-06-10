#pragma once

#include <cherry_macros.hpp>

#include "singleton.hpp"

#include "maths/vector3.hpp"

struct ALCcontext;
struct ALCdevice;


class CCENGINE_API SoundManager : public Singleton<SoundManager> 
{
private:
	ALCcontext*	m_context = nullptr;
	ALCdevice*	m_device = nullptr;

public:
	SoundManager();
	~SoundManager();

	static bool Init();
};