#pragma once

#include "cherry_macros.hpp"

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
	bool	m_isEnabled = true;;
	bool	m_isKinematic = false;
	bool	m_useGravity = true;

	Bool3	m_positionConstraints;
	Bool3	m_rotationConstraints;

	Vector3 m_velocity			= Vector3::Zero;
	Vector3 m_angularVelocity	= Vector3::Zero;
	Vector3 m_InertiaTensor		= Vector3::Zero;

	float	m_density = 10.0f;
	float	m_maxLinearVelocity			= (float)1.00000003e+16;
	float	m_maxAngularVelocity			= 100.0f;
	float	m_maxDepenetrationVelocity	= (float)1.00000003e+32;

	void PopulateMetadatas() override;

public:
	PhysicSystem::PhysicActor* m_physicActor = nullptr;
	
	Rigidbody();
	~Rigidbody();

	void	BindToSignals() override;
	void	Unregister();

	void	SetPxActor();
	void	SetActorConstraints();
	void	SetActorEnabled();
	void	SetActorKinematic();
	void	SetActorGravity();
	void	SetActorMaxVelocities();
	void	SetActorDensity();

	void	SetEnabled(const bool& isEnabled) { m_isEnabled = isEnabled; SetActorEnabled(); }
	bool	GetEnabled() { return m_isEnabled; }
	void	SetKinematic(const bool& isKinematic) { m_isKinematic = isKinematic; SetActorKinematic(); }
	bool	GetKinematic() { return m_isKinematic; }
	void	SetGravity(const bool& useGravity) { m_useGravity = useGravity; SetActorGravity(); }
	bool	GetGravity() { return m_useGravity; }
	
	void	SetPosContraints(const Bool3& constraints) { m_positionConstraints = constraints; SetActorConstraints(); }
	Bool3	GetPosConstraints() { return m_positionConstraints; }
	void	SetRotContraints(const Bool3& constraints) { m_rotationConstraints = constraints; SetActorConstraints(); }
	Bool3	GetRotConstraints() { return m_rotationConstraints; }

	void	SetDensity(const float& density) { m_density = density; SetActorDensity(); }
	float	GetDensity() { return m_density; }
	void	SetMaxVel(const float& density) { m_maxLinearVelocity = density; SetActorMaxVelocities(); }
	float	GetMaxVel() { return m_maxLinearVelocity; }
	void	SetMaxAngVel(const float& density) { m_maxAngularVelocity = density; SetActorMaxVelocities(); }
	float	GetMaxAngVel() { return m_maxAngularVelocity; }
	void	SetMaxDepVel(const float& density) { m_maxDepenetrationVelocity = density; SetActorMaxVelocities(); }
	float	GetMaxDepVel() { return m_maxDepenetrationVelocity; }

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