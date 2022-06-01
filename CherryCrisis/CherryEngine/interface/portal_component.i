%{
	#include "portal_component.hpp"
%}

%include behaviour.i

%nodefaultctor PortalComponent;

class PortalComponent : public Behaviour
{
public:
	%immuable;
	Transform* m_transform = nullptr;

	void SetLinkedPortal(Behaviour* linkedObject);
	Behaviour* GetLinkedPortal();
};
