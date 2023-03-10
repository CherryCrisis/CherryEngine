#pragma once

#include "behaviour.hpp"
#include "bool3.hpp"
#include "maths.hpp"

class Transform;

namespace PhysicSystem
{
	class PhysicActor;
	enum class EForceMode;
}

namespace physx
{
	class PxRigidDynamic;
}

using namespace CCMaths;


class CCENGINE_API Rigidbody : public Behaviour
{
private:
	using bool3Property = CCProperty::ConstRefProperty<Rigidbody, Bool3>;
	using boolProperty	= CCProperty::ConstRefProperty<Rigidbody, bool>;
	using floatProperty = CCProperty::ConstRefProperty<Rigidbody, float>;

	bool	m_isRegistered = false;
	bool	m_isEnabled = true;
	bool	m_useGravity = true;
	bool	m_isKinematic = false;

	float	m_density = 10.0f;
	float	m_maxLinearVelocity = 100.f;
	float	m_maxAngularVelocity = 7.0f;
	float	m_maxDepenetrationVelocity = 100.f;

	Bool3	m_positionConstraints = {};
	Bool3	m_rotationConstraints = {};

	Vector3 m_SavedVelocity = Vector3::Zero;

	Transform* m_transform = nullptr;
	physx::PxRigidDynamic* m_dynamicActor = nullptr;

	void PopulateMetadatas() override;

public:
	PhysicSystem::PhysicActor* m_physicActor = nullptr;
	
	Rigidbody();
	Rigidbody(CCUUID& id);
	~Rigidbody();

	void	BindToSignals() override;
	void	Initialize();
	void	Start();

	//! Unregister the rigidbody from the Physic Manager
	/*!
	*	Unregister the rigidbody from the Physic Manager
	*/
	void	Unregister(bool checkEmpty = true);

	void	SetEnabled();

	void SaveVelocity();
	void ReapplyVelocity();
	void SetVelocity(const Vector3& vel);
	// Set and gets used for the properties and Serialization
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

	void AddForce(const CCMaths::Vector3& force, PhysicSystem::EForceMode mode);
	void AddTorque(const CCMaths::Vector3& force, PhysicSystem::EForceMode mode);

	boolProperty	isEnabled	{ this, &Rigidbody::SetEnabled,   &Rigidbody::GetEnabled   };
	boolProperty	isKinematic	{ this, &Rigidbody::SetKinematic, &Rigidbody::GetKinematic };
	boolProperty	useGravity	{ this, &Rigidbody::SetGravity,   &Rigidbody::GetGravity   };
	
	bool3Property	positionConstraints{ this, &Rigidbody::SetPosContraints, &Rigidbody::GetPosConstraints };
	bool3Property	rotationConstraints{ this, &Rigidbody::SetRotContraints, &Rigidbody::GetRotConstraints };

	floatProperty	density						{ this, &Rigidbody::SetDensity,		&Rigidbody::GetDensity	 };
	floatProperty	maxAngularVelocity			{ this, &Rigidbody::SetMaxAngVel,	&Rigidbody::GetMaxAngVel };
	floatProperty	maxDepenetrationVelocity	{ this, &Rigidbody::SetMaxDepVel,	&Rigidbody::GetMaxDepVel };
	floatProperty	maxLinearVelocity			{ this, &Rigidbody::SetMaxVel,		&Rigidbody::GetMaxVel	 };
};