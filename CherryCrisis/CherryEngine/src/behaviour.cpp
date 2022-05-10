#include <pch.hpp>

#include "behaviour.hpp"
#include "entity.hpp"

void Behaviour::SetHostPtr(Entity* newOwner)
{
	newOwner->SubscribeComponent(this);
}