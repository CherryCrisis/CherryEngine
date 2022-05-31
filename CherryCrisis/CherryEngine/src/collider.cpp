#include <pch.hpp>

#include "collider.hpp"

#include <PxPhysicsAPI.h>


void Collider::SetPxLocalPos()
{
	physx::PxTransform transform = m_pxShape->getLocalPose();
	transform.p = physx::PxVec3(m_localPosition.x, m_localPosition.y, m_localPosition.z);
	m_pxShape->setLocalPose(transform);
}

void Collider::SetEnabled(const bool& isEnabled)
{
	m_isEnabled = isEnabled;
	SetPxData();
}

void Collider::SetTrigger(const bool& isTrigger)
{
	m_isTrigger = isTrigger;
	SetPxData();
}

void Collider::SetLocalPos(const CCMaths::Vector3& localPos)
{
	m_localPosition = localPos;
	ComputeModelMatrices();

	if (m_pxShape)
		SetPxLocalPos();
}

void Collider::Copy(Behaviour* copy)
{
	Collider* copiedCollider = dynamic_cast<Collider*>(copy);

	m_isRegistered = copiedCollider->m_isRegistered;
	m_isEnabled = copiedCollider->m_isEnabled;
	m_isTrigger = copiedCollider->m_isTrigger;
	m_isAddedFromInspector = copiedCollider->m_isAddedFromInspector;

	m_localPosition = copiedCollider->m_localPosition;
	m_model = copiedCollider->m_model;

	ResetPxShape();
}