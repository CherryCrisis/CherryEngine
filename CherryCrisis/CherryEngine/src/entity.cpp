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
	for (auto& [type, behaviour] : m_behaviours)
		delete behaviour;
}

void Entity::Start()
{
	
}

void Entity::Update()
{
	m_OnStart.Invoke();
	m_OnTick.Invoke();
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

std::vector<Behaviour*> Entity::GetAllBehaviours()
{
	std::vector<Behaviour*> behaviours;

	for (auto& [type, behaviour] : m_behaviours)
		behaviours.push_back(behaviour);

	return behaviours;
}

std::string Entity::Serialized() 
{
	std::string value;
	value += "Entity:"+std::to_string(m_uuid)+"\n";
	value += "  m_name:"+m_name+"\n";
	value += "	m_components:\n";
	for (auto& [type, behaviour] : m_behaviours)
	{
		value += "	-:" + std::to_string(behaviour->GetUUID())+ "\n";
	}

	return value.c_str();
}

std::string Entity::SerializeBehaviours() 
{
	std::string value;

	for (auto& [type, behaviour] : m_behaviours)
	{
		value += "Behaviour:"+std::to_string(behaviour->GetUUID()) + "\n";
		value += "m_type:"+ std::string(typeid(*behaviour).name()) + "\n";
		value += behaviour->Serialize() + "\n";
	}

	return value.c_str();
}