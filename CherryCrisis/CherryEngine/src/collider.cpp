#include "pch.hpp"

#include "collider.hpp"

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