%{
	#include "collider.hpp"
%}

%nodefaultctor Collider;

class Collider : public Behaviour
{
public:
	PhysicSystem::PhysicActor* m_physicActor;

	void	SetEnabled(const bool& isEnabled);
	bool	GetEnabled();
	void	SetTrigger(const bool& isTrigger);
	bool	GetTrigger();
};