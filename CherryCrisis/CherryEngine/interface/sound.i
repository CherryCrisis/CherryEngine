%{
	#include "sound.hpp"
%}

%include vector3.i

%nodefaultctor Sound;

%include <std_shared_ptr.i>
%shared_ptr(Sound)

class Sound
{
public:
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