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
	using boolProperty		= CCProperty::ConstRefProperty<Collider, bool>;
	using floatProperty		= CCProperty::ConstRefProperty<Collider, float>;
	using Vector3Property	= CCProperty::ConstRefProperty<Collider, CCMaths::Vector3>;

	bool	m_isRegistered = false;
	bool	m_isEnabled = true;
	bool	m_isTrigger = false;
	bool	m_isAddedFromInspector = false;
	bool	m_isBlockingRaycast	= true;

	CCMaths::Vector3 m_localPosition = CCMaths::Vector3::Zero;
	CCMaths::Matrix4 m_model = CCMaths::Matrix4::Identity;

	physx::PxShape*		m_pxShape = nullptr;
	class Transform*	m_transform = nullptr;

	virtual void PopulateMetadatas() override {}

public:
	float m_isVisible = false;

	EColliderType m_type = EColliderType::NONE;
	
	PhysicSystem::PhysicActor* m_physicActor = nullptr;

	Collider() = default;
	Collider(CCUUID& id) : Behaviour(id) {}

	virtual void BindToSignals() override {}
	virtual void Initialize() {}
	virtual void InvalidateTransform() {}	// Set pointer on transform to nullptr if transform is deleted to avoid crash

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
	virtual void SetEntityScale(Transform* transform) {}

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

	// Set the local transform of the PxShape relative to the PxActor
	void	SetPxLocalPos();

	virtual void Copy(Behaviour* copy) override;
	
	// Set if collider debug is displayed
	virtual void Visible()		{ m_isVisible = true; }
	virtual void Unvisible()	{ m_isVisible = false; }

	// Set and gets used for the properties and Serialization
	void	SetEnabled(const bool& isEnabled);
	bool	GetEnabled() { return m_isEnabled; }
	void	SetTrigger(const bool& isTrigger);
	bool	GetTrigger() { return m_isTrigger; }
	
	void	SetBlockRaycast(const bool& isBlocking);
	bool	GetBlockRaycast() { return m_isBlockingRaycast; }

	void				SetLocalPos(const CCMaths::Vector3& localPos);
	CCMaths::Vector3	GetLocalPos() { return m_localPosition; }


	virtual void RecomputeMatrix(Transform* transform) = 0;	// Called by a transform event, calls the following function
	virtual void ComputeModelMatrices() = 0;				// Calculate collider display model matrix
	virtual CCMaths::Matrix4 GetModelMatrix() = 0;

	boolProperty isEnabled			{ this, &Collider::SetEnabled,		&Collider::GetEnabled };
	boolProperty isTrigger			{ this, &Collider::SetTrigger,		&Collider::GetTrigger };
	boolProperty isBlocking			{ this, &Collider::SetBlockRaycast, &Collider::GetBlockRaycast };
	Vector3Property localPosition	{ this, &Collider::SetLocalPos,		&Collider::GetLocalPos };
};