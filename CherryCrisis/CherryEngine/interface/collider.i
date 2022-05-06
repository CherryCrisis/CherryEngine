%{
	#include "collider.hpp"
%}

%include vector.i

class Collider : public Behaviour
{
private:
	bool	m_isEnabled;
	bool	m_isTrigger;

public:
	PhysicSystem::PhysicActor* m_physicActor;

	Collider();

	%ignore Collider();
};