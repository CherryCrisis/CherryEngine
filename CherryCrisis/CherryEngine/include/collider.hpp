#pragma once

#include "behaviour.hpp"

namespace physx
{
	class PxShape;
}
namespace PhysicSystem
{
	class PhysicActor;
}

class CCENGINE_API Collider : public Behaviour
{
protected:
	using boolProperty = CCProperty::ConstRefProperty<Collider, bool>;
	using floatProperty = CCProperty::ConstRefProperty<Collider, float>;
	
	bool	m_isRegistered = false;
	bool	m_isEnabled = true;
	bool	m_isTrigger = false;
	float	m_contactOffset = 0.1f;

	virtual void PopulateMetadatas() override {}

public:
	PhysicSystem::PhysicActor* m_physicActor = nullptr;

	virtual void BindToSignals() override {}
	virtual void Unregister() {}
	virtual void SetEntityScale(const Vector3&) {}
	virtual void SetPxShape() {}
	virtual void ClearPxShape() {}
	virtual void ResetPxShape() {}

	virtual void SetPxData() {}

	void	SetEnabled(const bool& isEnabled);
	bool	GetEnabled() { return m_isEnabled; }
	void	SetTrigger(const bool& isTrigger);
	bool	GetTrigger() { return m_isTrigger; }
	void	SetContact(const float& contactOffset);
	float	GetContact() { return m_contactOffset; }

	boolProperty isEnabled{ this, &Collider::SetEnabled, &Collider::GetEnabled };
	boolProperty isTrigger{ this, &Collider::SetTrigger, &Collider::GetTrigger };
	floatProperty contactOffset{ this, &Collider::SetContact, &Collider::GetContact };
};