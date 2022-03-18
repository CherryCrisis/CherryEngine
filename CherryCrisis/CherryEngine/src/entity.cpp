#include "pch.hpp"

#include "entity.hpp"

#include "behaviour.hpp"
#include "transform.hpp"

#include <typeinfo>
Entity::Entity(const std::string& name)
	: m_name(name)
{

}

Entity::~Entity()
{
	for (Behaviour* behaviour : m_behaviours)
		delete behaviour;
}

void Entity::Start()
{
	for (Behaviour* behaviour : m_behaviours)
		behaviour->Start();
}

void Entity::Update()
{
	for (Behaviour* behaviour : m_behaviours)
		behaviour->Update();
}

void Entity::Destroy()
{
	Transform* transform;
	if (!TryGetBehaviour(transform))
		return;

	std::vector<Transform*> children = transform->GetChildren();

	for (int i = 0; i < children.size(); i++)
		children[i]->GetHost().Destroy();
}

std::string Entity::Serialized() 
{
	std::string value;
	value += "Entity:\n";
	value += "	m_name: "+m_name+"\n";
	value += "	m_components: \n";
	for (Behaviour* behaviour : m_behaviours) 
	{
		value += "	-: "+std::string(typeid(*behaviour).name())+"\n";
	}

	return value.c_str();
}