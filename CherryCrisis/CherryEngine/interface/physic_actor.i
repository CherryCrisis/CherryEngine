%{
	#include "physic_actor.hpp"
%}

%include maths.i
%include physic_manager.i

class PhysicActor
{
	Raycast(const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);
	AddForce(const CCMaths::Vector3& force, EForceMode mode);

	void	SetKinematic(const bool& isKinematic);
	bool	GetKinematic();
	void	SetEnabled(const bool& isEnabled);
	bool	GetEnabled();
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
};