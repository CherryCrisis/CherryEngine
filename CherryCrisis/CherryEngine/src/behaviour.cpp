#include <pch.hpp>

#include "behaviour.hpp"

void Behaviour::SetHostPtr(Entity* newOwner)
{
	newOwner->SubscribeComponent(this);
}