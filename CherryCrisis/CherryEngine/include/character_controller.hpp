#pragma once

#include "behaviour.hpp"

namespace physx
{
	class PxMaterial;
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

	CCMaths::Vector3 position;

	float m_radius			= 0.5f;
	float m_height			= 1.0f;
	float m_slopeLimit		= 0.707f;
	float m_maxJumpHeight	= 0.0f;
	float m_contactOffset	= 0.1f;
	float m_stepOffset		= 0.5f;
	float m_density			= 10.f;
		
	physx::PxMaterial* material = nullptr;

	void PopulateMetadatas() override;
	
public:
	CharacterController() {}

	void	SetRadius(const float& radius) { m_radius = radius; }
	float	GetRadius() { return m_radius; }
	void	SetHeight(const float& height) { m_height = height; }
	float	GetHeight() { return m_height; }
	void	SetSlopeLimit(const float& slopeLimit) { m_slopeLimit = slopeLimit; }
	float	GetSlopeLimit() { return m_slopeLimit; }
	void	SetMaxJumpHeight(const float& maxJumpHeight) { m_maxJumpHeight = maxJumpHeight; }
	float	GetMaxJumpHeight() { return m_maxJumpHeight; }

	void	SetContact(const float& contactOffset) { m_contactOffset = contactOffset; }
	float	GetContact() { return m_contactOffset; }
	void	SetStep(const float& stepOffset) { m_stepOffset = stepOffset; }
	float	GetStep() { return m_stepOffset; }
	void	SetDensity(const float& density) { m_density = density; }
	float	GetDensity() { return m_density; }

	floatProperty	radius			{ this, &CharacterController::SetRadius,		&CharacterController::GetRadius  };
	floatProperty	height			{ this, &CharacterController::SetHeight,		&CharacterController::GetHeight	 };
	floatProperty	slopeLimit		{ this, &CharacterController::SetSlopeLimit,	&CharacterController::GetSlopeLimit };
	floatProperty	maxJumpHeight	{ this, &CharacterController::SetMaxJumpHeight,	&CharacterController::GetMaxJumpHeight };
	floatProperty	contactOffset	{ this, &CharacterController::SetContact,		&CharacterController::GetContact };
	floatProperty	stepOffset		{ this, &CharacterController::SetStep,			&CharacterController::GetStep };
	floatProperty	density			{ this, &CharacterController::SetDensity,		&CharacterController::GetDensity };
};