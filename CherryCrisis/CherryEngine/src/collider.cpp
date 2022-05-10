#include "pch.hpp"

#include "collider.hpp"

#include <PxPhysicsAPI.h>

void Collider::SetPxLocalPos()
{
	physx::PxTransform transform = m_pxShape->getLocalPose();
	physx::PxTransform relativePos = physx::PxTransform(physx::PxVec3(m_localPosition.x, m_localPosition.y, m_localPosition.z));
	m_pxShape->setLocalPose(transform * relativePos);
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

void Collider::SetContact(const float& contactOffset)
{
	m_contactOffset = contactOffset;
	SetPxData();
}

void Collider::SetLocalPos(const CCMaths::Vector3& localPos)
{
	m_localPosition = localPos;

	if (m_pxShape)
		SetPxLocalPos();
}
