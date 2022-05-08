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

	bool	m_isRegistered = false;
	
	// TODO: Probably remove
	float m_contactOffset	= 0.1f;

	void PopulateMetadatas() override;
	
public:
	PhysicSystem::PhysicActor* m_physicActor = nullptr;

	CharacterController() {}
	CharacterController(CCUUID& id);
	~CharacterController();

	void BindToSignals() override;
	void Unregister();

	void Update();

	void	SetContact(const float& contactOffset) { m_contactOffset = contactOffset; }
	float	GetContact() { return m_contactOffset; }

	floatProperty	contactOffset	{ this, &CharacterController::SetContact, &CharacterController::GetContact };
};