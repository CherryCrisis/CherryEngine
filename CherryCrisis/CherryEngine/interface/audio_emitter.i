%{
	#include "audio_emitter.hpp"
%}

%include behaviour.i
%include sound.i

%nodefaultctor AudioEmitter;

class AudioEmitter : public Behaviour
{
public:
	%immuable;
	Transform* m_transform = nullptr;

	void Play();
	void Pause();
	void Stop();
	
	void SetLooping(const bool& loop);
	void SetPitch(const float& value);
	void SetRollOff(const float& value);
	void SetReferenceDistance(const float& value);
	void SetSpatialized(const bool& value);

	void AddSound(const std::string& filepath);
	void AddSound(std::shared_ptr<Sound> sound);

	bool	    GetLooping();
	bool	    GetSpatialized();
	float	    GetRollOff();

	float	    GetReferenceDistance();
	float	    GetPitch();
	std::string GetSoundPath();
};
