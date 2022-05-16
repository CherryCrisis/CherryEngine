#pragma once

#include "resource.hpp"
#include "cherry_macros.hpp"

#include "maths/vector3.hpp"

class CCENGINE_API Sound : public Resource<Sound>
{
private:
	unsigned int m_buffer = 0u;
	unsigned int m_source = 0u;

protected:

public:
	Sound(const char* filepath) : Resource(filepath) {}
	
	static void Load(std::shared_ptr<Sound> sound);

	void Delete() override;
	void Reload() {}

	void Play();
	void Pause();
	void Stop();

	void SetLooping(bool loop);
	void SetPosition(float x, float y, float z);
	void SetPosition(const CCMaths::Vector3& pos);
	void SetDirection(const CCMaths::Vector3& fwd);
	void SetRollOff(float rolloff);
	void SetReferenceDistance(float distance);
	void SetPitch(float pitch);
};