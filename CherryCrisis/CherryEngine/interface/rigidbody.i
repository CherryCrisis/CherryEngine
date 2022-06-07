%{
	#include "rigidbody.hpp"

	using namespace PhysicSystem;
%}

%include behaviour.i
%include physic_manager.i

%nodefaultctor Rigidbody;

class Rigidbody : public Behaviour
{
public:
	%immutable;
	PhysicSystem::PhysicActor* m_physicActor;

	// Set and gets used for the properties and Serialization
	void	SetEnabled(const bool& isEnabled);
	bool	GetEnabled();
	void	SetKinematic(const bool& isKinematic);
	bool	GetKinematic();
	void	SetGravity(const bool& useGravity);
	bool	GetGravity();

	void	SetDensity(const float& density);
	float	GetDensity();
	void	SetMaxVel(const float& velocity);
	float	GetMaxVel();
	void	SetMaxAngVel(const float& velocity);
	float	GetMaxAngVel();
	void	SetMaxDepVel(const float& velocity);
	float	GetMaxDepVel();

	void AddForce(const CCMaths::Vector3& force, PhysicSystem::EForceMode mode);
	void AddTorque(const CCMaths::Vector3& force, PhysicSystem::EForceMode mode);
};