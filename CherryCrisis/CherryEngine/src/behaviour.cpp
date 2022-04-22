#include <pch.hpp>

// TODO: Change this
#include "behaviour.hpp"

void Behaviour::SetHostPtr(Entity* newOwner)
{
	m_owner = newOwner; 
	m_owner->SubscribeComponent(this);
}