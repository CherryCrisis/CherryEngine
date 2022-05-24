#include <pch.hpp>

#include "behaviour.hpp"
#include "entity.hpp"

const std::string Behaviour::TypeName()
{
	return String::ExtractTypeIndexName(typeid(*this));
}

void Behaviour::OnSetOwner(Entity* newOwner)
{
	newOwner->SubscribeComponent(this, TypeName());
}

void Behaviour::SetHostPtr(Entity* newOwner)
{
	OnSetOwner(newOwner);
}
