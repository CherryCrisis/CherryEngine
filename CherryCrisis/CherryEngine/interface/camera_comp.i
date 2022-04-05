%{
	#include "camera_component.hpp"
%}

%include vector.i

class CameraComponent : public Behaviour
{
public:
	CameraComponent();

	%ignore CameraComponent();

	Transform*	m_transform = nullptr;
};