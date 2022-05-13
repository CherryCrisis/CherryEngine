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

class CCENGINE_API CharacterController : public Behaviour
{
private:
	using boolProperty = CCProperty::ConstRefProperty<CharacterController, bool>;
	using floatProperty = CCProperty::ConstRefProperty<CharacterController, float>;

	class CapsuleCollider*	m_collider = nullptr;
	class Rigidbody*		m_rigidbody = nullptr;
	class Transform*		m_transform = nullptr;

	class TimeManager*	m_timeManager	= nullptr;

	bool	m_isRegistered = false;
	bool	m_isGrounded = false;
	
	float	m_moveSpeed = 10.f;
	float	m_contactOffset = 0.2f;
	
	float	m_springStrength = 150.f;
	float	m_springDampling = 20.f;

	void PopulateMetadatas() override;
	
public:
	PhysicSystem::PhysicActor* m_physicActor = nullptr;

	CharacterController();
	CharacterController(CCUUID& id);
	~CharacterController();

	void Initialize();

	void BindToSignals() override;
	void Unregister();

	void Update();

	void	SetSpeed(const float& moveSpeed) { m_moveSpeed = moveSpeed; }
	float	GetSpeed() { return m_moveSpeed; }

	void	SetSpringStrength(const float& springStrength) { m_springStrength = springStrength; }
	float	GetSpringStrength() { return m_springStrength; }

	void	SetSpringDampling(const float& springDampling) { m_springDampling = springDampling; }
	float	GetSpringDampling() { return m_springDampling; }

	floatProperty	moveSpeed		{ this, &CharacterController::SetSpeed,				&CharacterController::GetSpeed };
	floatProperty	springStrength	{ this, &CharacterController::SetSpringStrength,	&CharacterController::GetSpringStrength };
	floatProperty	springDampling	{ this, &CharacterController::SetSpringDampling,	&CharacterController::GetSpringDampling };

	void InvalidateTransform();
};