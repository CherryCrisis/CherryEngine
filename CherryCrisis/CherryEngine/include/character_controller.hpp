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


class CCENGINE_API CharacterController : public Behaviour
{
private:
	using boolProperty	= CCProperty::ConstRefProperty<CharacterController, bool>;
	using floatProperty	= CCProperty::ConstRefProperty<CharacterController, float>;

	bool	m_isStarted = false;
	bool	m_isGrounded = false;
	bool	m_isRegistered = false;
	float	m_contactOffset = 0.1f;
	float	m_moveSpeed = 10.f;
	float	m_springStrength = 150.f;
	float	m_springDampling = 20.f;
	float	m_forwardMove = 0.f;
	float	m_sideMove = 0.f;
	float	m_sensitivity = 1.f;

	CCMaths::Vector3	m_rotating = 0.f;

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

	void Initialize();

	void BindToSignals() override;
	void Unregister();

	void Update();
	void FixedUpdate();

	void Freeze();

	void InvalidateTransform();

	void	SetSpeed(const float& moveSpeed) { m_moveSpeed = moveSpeed; }
	float	GetSpeed() { return m_moveSpeed; }
	
	void	SetSensitivity(const float& sensitivity) { m_sensitivity = sensitivity; }
	float	GetSensitivity() { return m_sensitivity; }

	void	SetSpringStrength(const float& springStrength) { m_springStrength = springStrength; }
	float	GetSpringStrength() { return m_springStrength; }

	void	SetSpringDampling(const float& springDampling) { m_springDampling = springDampling; }
	float	GetSpringDampling() { return m_springDampling; }

	floatProperty	moveSpeed		{ this, &CharacterController::SetSpeed,				&CharacterController::GetSpeed };
	floatProperty	sensitivity		{ this, &CharacterController::SetSensitivity,		&CharacterController::GetSensitivity };
	floatProperty	springStrength	{ this, &CharacterController::SetSpringStrength,	&CharacterController::GetSpringStrength };
	floatProperty	springDampling	{ this, &CharacterController::SetSpringDampling,	&CharacterController::GetSpringDampling };
};