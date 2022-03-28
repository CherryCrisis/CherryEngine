#include "pch.hpp"

#include "entity.hpp"

#include "behaviour.hpp"
#include "transform.hpp"

#include <typeinfo>
Entity::Entity(const std::string& name, CCUUID id)
	: m_name(name)
{
	m_uuid = id;
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
	value += "Entity:"+std::to_string(m_uuid)+"\n";
	value += "  m_name:"+m_name+"\n";
	value += "	m_components:\n";
	for (Behaviour* behaviour : m_behaviours) 
	{
		value += "	-:" + std::to_string(behaviour->GetUUID())+ "\n";
	}

	return value.c_str();
}

std::string Entity::SerializeBehaviours() 
{
	std::string value;

	for (Behaviour* behaviour : m_behaviours)
	{
		value += "Behaviour:"+std::to_string(behaviour->GetUUID()) + "\n";
		value += "m_type:"+ std::string(typeid(*behaviour).name()) + "\n";
		value += behaviour->Serialize() + "\n";
	}

	return value.c_str();
}

void Entity::SubscribeComponent(Behaviour* behaviour)
{
	behaviour->m_owner = this;
	m_behaviours.push_back(behaviour);
}