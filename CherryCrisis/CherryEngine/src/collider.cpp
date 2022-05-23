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

	if (m_pxShape)
		SetPxLocalPos();
}
