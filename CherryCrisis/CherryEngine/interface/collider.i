%{
	#include "collider.hpp"
%}

class Collider : public Behaviour
{
public:
	PhysicSystem::PhysicActor* m_physicActor;

	Collider();

	%ignore Collider();

	void	SetEnabled(const bool& isEnabled);
	bool	GetEnabled();
	void	SetTrigger(const bool& isTrigger);
	bool	GetTrigger();
};