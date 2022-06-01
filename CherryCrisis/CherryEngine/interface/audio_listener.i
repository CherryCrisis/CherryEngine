%{
	#include "audio_listener.hpp"
%}

%include behaviour.i

%nodefaultctor AudioListener;

class AudioListener : public Behaviour
{
public:
	%immuable;
	Transform* m_transform = nullptr;
};