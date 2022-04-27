#pragma once

#include "behaviour.hpp"
#include "maths.hpp"
#include "bool3.hpp"

class Transform;
namespace PhysicSystem
{
	class PhysicActor;
}

using namespace CCMaths;

class CCENGINE_API Rigidbody : public Behaviour
{
private:
	using bool3Property = CCProperty::ConstRefProperty<Rigidbody, Bool3>;
	using boolProperty = CCProperty::ConstRefProperty<Rigidbody, bool>;
	using floatProperty = CCProperty::ConstRefProperty<Rigidbody, float>;

	bool	m_isRegistered = false;

	void PopulateMetadatas() override;

public:
	PhysicSystem::PhysicActor* m_physicActor = nullptr;
	
	Rigidbody();
	Rigidbody(CCUUID& id);
	~Rigidbody();

	void	BindToSignals() override;
	void	Unregister();

	void	SetEnabled(const bool& isEnabled);
	bool	GetEnabled();
	void	SetKinematic(const bool& isKinematic);
	bool	GetKinematic();
	void	SetGravity(const bool& useGravity);
	bool	GetGravity();
	
	void	SetPosContraints(const Bool3& constraints);
	Bool3	GetPosConstraints();
	void	SetRotContraints(const Bool3& constraints);
	Bool3	GetRotConstraints();

	void	SetDensity(const float& density);
	float	GetDensity();
	void	SetMaxVel(const float& velocity);
	float	GetMaxVel();
	void	SetMaxAngVel(const float& velocity);
	float	GetMaxAngVel();
	void	SetMaxDepVel(const float& velocity);
	float	GetMaxDepVel();
	
	boolProperty	isEnabled	{ this, &Rigidbody::SetEnabled,   &Rigidbody::GetEnabled   };
	boolProperty	isKinematic	{ this, &Rigidbody::SetKinematic, &Rigidbody::GetKinematic };
	boolProperty	useGravity	{ this, &Rigidbody::SetGravity,   &Rigidbody::GetGravity   };
	
	bool3Property positionConstraints{ this, &Rigidbody::SetPosContraints, &Rigidbody::GetPosConstraints };
	bool3Property rotationConstraints{ this, &Rigidbody::SetRotContraints, &Rigidbody::GetRotConstraints };

	floatProperty	density						{ this, &Rigidbody::SetDensity,		&Rigidbody::GetDensity	 };
	floatProperty	maxLinearVelocity			{ this, &Rigidbody::SetMaxVel,		&Rigidbody::GetMaxVel	 };
	floatProperty	maxAngularVelocity			{ this, &Rigidbody::SetMaxAngVel,	&Rigidbody::GetMaxAngVel };
	floatProperty	maxDepenetrationVelocity	{ this, &Rigidbody::SetMaxDepVel,	&Rigidbody::GetMaxDepVel };
};