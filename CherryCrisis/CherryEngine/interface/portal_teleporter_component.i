%{
	#include "portal_teleporter_component.hpp"
%}

%include behaviour.i

%nodefaultctor PortalComponent;

class PortalTeleporterComponent : public Behaviour
{
public:
	void ReloadEntitiesClone();

	%immutable;
	Transform* m_transform = nullptr;
};
