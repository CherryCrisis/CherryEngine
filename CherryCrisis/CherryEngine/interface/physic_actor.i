%{
	#include "physic_actor.hpp"

	using namespace PhysicSystem;
%}

%include maths.i
%include entity.i

%nodefaultctor PhysicActor;

class PhysicActor
{
public:
	void AddForce(const CCMaths::Vector3& force, EForceMode mode);
	void AddTorque(const CCMaths::Vector3& force, EForceMode mode);
	RaycastHit Raycast(const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);

	void	SetEnabled(const bool& isEnabled);
	bool	GetEnabled();

public:
	%immutable;
	Entity* m_owner = nullptr;

	//Rigidbody* GetRigidbody();
	bool HasRigidbody();
	bool HasColliders();
};