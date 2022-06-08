#pragma once

#include "behaviour.hpp"

namespace PhysicSystem
{
	class PhysicActor;
}

namespace CCMaths
{
	struct Vector3;
}

namespace physx
{
	class PxRigidDynamic;
}


struct AlignementData
{
	bool	m_isRotating = false;
	float	m_lerpPercent = 0.0f;
	
	CCMaths::Vector3	m_to = CCMaths::Vector3::Up;

	CCMaths::Quaternion m_startRotation;
	CCMaths::Quaternion m_goalRotation;
};

class CCENGINE_API CharacterController : public Behaviour
{
private:
	using boolProperty	= CCProperty::ConstRefProperty<CharacterController, bool>;
	using floatProperty	= CCProperty::ConstRefProperty<CharacterController, float>;

	bool	m_isStarted		 = false;
	bool	m_isGrounded	 = false;
	bool	m_isRegistered	 = false;
	bool	m_isRunning		= false;

	float	m_contactOffset  = 0.1f;
	float	m_walkingSpeed   = 1.5f;
	float	m_runningSpeed   = 2.5f;
	float	m_springStrength = 150.f;
	float	m_springDampling = 20.f;
	float	m_forwardMove	 = 0.f;
	float	m_sideMove		 = 0.f;
	float	m_sensitivity	 = 1.f;
	float	m_jumpForce		 = 1.f;
	float	m_rotating		 = 0.f;

	CCMaths::Vector3 m_raycastDir = -CCMaths::Vector3::Up;

	AlignementData m_alignement = {};
	physx::PxRigidDynamic*	m_dynamicActor = nullptr;

	void PopulateMetadatas() override;
	
public:
	class CapsuleCollider*		m_collider = nullptr;
	class Rigidbody*			m_rigidbody = nullptr;
	class Transform*			m_transform = nullptr;
	PhysicSystem::PhysicActor*	m_physicActor = nullptr;

	CharacterController();
	CharacterController(CCUUID& id);
	~CharacterController();

	void Start();
	void Initialize();

	void BindToSignals() override;
	void Unregister(bool checkEmpty = true);

	void Update();
	void FixedUpdate();
	void AlignToGravity();	// Rotate the Actor so it's vertical axis is align with gravity (in case it gets out of a portal on the side)

	void InvalidateTransform();	// Set pointer on transform to nullptr if transform is deleted to avoid crash

	// Set and gets used for the properties and Serialization
	void	SetWalkingSpeed(const float& moveSpeed) { m_walkingSpeed = moveSpeed; }
	float	GetWalkingSpeed() { return m_walkingSpeed; }
	
	void	SetRunningSpeed(const float& moveSpeed) { m_runningSpeed = moveSpeed; }
	float	GetRunningSpeed() { return m_runningSpeed; }

	void	SetJumpForce(const float& jumpForce) { m_jumpForce = jumpForce; }
	float	GetJumpForce() { return m_jumpForce; }

	void	SetSensitivity(const float& sensitivity) { m_sensitivity = sensitivity; }
	float	GetSensitivity() { return m_sensitivity; }

	void	SetSpringStrength(const float& springStrength) { m_springStrength = springStrength; }
	float	GetSpringStrength() { return m_springStrength; }

	void	SetSpringDampling(const float& springDampling) { m_springDampling = springDampling; }
	float	GetSpringDampling() { return m_springDampling; }

	void	SetRaycastDir(const CCMaths::Vector3& dir) { m_raycastDir = dir; m_alignement.m_to = -dir; }

	floatProperty	runningSpeed    { this, &CharacterController::SetRunningSpeed,		&CharacterController::GetRunningSpeed   };
	floatProperty	walkingSpeed    { this, &CharacterController::SetWalkingSpeed,		&CharacterController::GetWalkingSpeed   };
	floatProperty	sensitivity		{ this, &CharacterController::SetSensitivity,		&CharacterController::GetSensitivity    };
	floatProperty	springStrength	{ this, &CharacterController::SetSpringStrength,	&CharacterController::GetSpringStrength };
	floatProperty	springDampling	{ this, &CharacterController::SetSpringDampling,	&CharacterController::GetSpringDampling };
	floatProperty	jumpForce    	{ this, &CharacterController::SetJumpForce,	        &CharacterController::GetJumpForce	    };
};