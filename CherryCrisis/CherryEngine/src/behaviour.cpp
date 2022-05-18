#include <pch.hpp>

#include "behaviour.hpp"
#include "entity.hpp"

void Behaviour::OnSetOwner(Entity* newOwner)
{
	newOwner->SubscribeComponent(this);
}

void Behaviour::SetHostPtr(Entity* newOwner)
{
	OnSetOwner(newOwner);
}