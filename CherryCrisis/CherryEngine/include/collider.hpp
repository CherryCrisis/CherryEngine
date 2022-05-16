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

enum class EColliderType
{
	NONE,
	BOX,
	SPHERE,
	CAPSULE,
};

class CCENGINE_API Collider : public Behaviour
{
protected:
	using boolProperty = CCProperty::ConstRefProperty<Collider, bool>;
	using floatProperty = CCProperty::ConstRefProperty<Collider, float>;
	using Vector3Property = CCProperty::ConstRefProperty<Collider, CCMaths::Vector3>;

	physx::PxShape* m_pxShape = nullptr;
	class Transform* m_transform = nullptr;

	bool	m_isRegistered = false;
	bool	m_isEnabled = true;
	bool	m_isTrigger = false;
	float	m_contactOffset = 0.02f;

	CCMaths::Vector3 m_localPosition = CCMaths::Vector3::Zero;

	virtual void PopulateMetadatas() override {}

public:
	PhysicSystem::PhysicActor* m_physicActor = nullptr;
	EColliderType m_type = EColliderType::NONE;
	float m_isVisible = true;

	virtual void BindToSignals() override {}
	virtual void Initialize() {}
	virtual void InvalidateTransform() {}

	//! Unregister collider from the Physic Manager
	/*!
	*	Unregister collider from the Physic Manager
	*/
	virtual void Unregister() {}

	//! Set collider scale
	/*!
	*	\param scale: scale of the entity transform
	* 
	*	Apply the scale of the entity as base scale of the collider
	*/
	virtual void SetEntityScale(const CCMaths::Vector3& scale) {}

	//! Create a pxShape
	/*!
	*	Create a pxShape for the collider in the pxActor of the entity
	*/
	virtual void SetPxShape() {}

	//! Delete the pxShape
	/*!
	*	Delete the pxShape for the collider in the pxActor of the entity
	*/
	virtual void ClearPxShape() {}

	//! Delete and re-create the pxShape
	/*!
	*	Reset the pxShape by calling ClearPxShape() then SetPxShape()
	*/
	virtual void ResetPxShape() {}

	//! Setup the pxShape from PhysX
	/*!
	*   Setup the pxShape from PhysX
	*/
	virtual void SetPxData() {}

	void	SetPxLocalPos();

	void	SetEnabled(const bool& isEnabled);
	bool	GetEnabled() { return m_isEnabled; }
	void	SetTrigger(const bool& isTrigger);
	bool	GetTrigger() { return m_isTrigger; }
	void	SetContact(const float& contactOffset);
	float	GetContact() { return m_contactOffset; }

	void				SetLocalPos(const CCMaths::Vector3& localPos);
	CCMaths::Vector3	GetLocalPos() { return m_localPosition; }

	virtual CCMaths::Matrix4 GetTranformMatrix() = 0;

	boolProperty isEnabled{ this, &Collider::SetEnabled, &Collider::GetEnabled };
	boolProperty isTrigger{ this, &Collider::SetTrigger, &Collider::GetTrigger };
	floatProperty contactOffset{ this, &Collider::SetContact, &Collider::GetContact };
	Vector3Property localPosition{ this, &Collider::SetLocalPos, &Collider::GetLocalPos };

	Collider() = default;
	Collider(CCUUID& id) : Behaviour(id) {}
};