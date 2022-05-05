#include "pch.hpp"

#include "entity.hpp"

#include "behaviour.hpp"
#include "transform.hpp"
#include "cell_system.hpp"

#include <typeinfo>
Entity::Entity()
{
	CellSystem::GetInstance()->AddEntityToDefault(this);
}

Entity::Entity(const std::string& name, CCUUID id)
	: Object(id), m_name(name)
{
	CellSystem::GetInstance()->AddEntityToDefault(this);
}


Entity::~Entity()
{
	for (auto& [type, behaviour] : m_behaviours)
		delete behaviour;

	m_cell->RemoveEntity(this);
}

void Entity::Initialize() 
{
	for (auto& [type, behaviour] : m_behaviours)
		behaviour->BindToSignals();
	
	m_OnAwake.Invoke();	
}	

bool Entity::RemoveBehaviour(Behaviour* behaviour)
{
	auto compIt = m_behaviours.find(typeid(*behaviour));

	if (compIt == m_behaviours.end())
	{
		compIt = m_behaviours.find(typeid(Behaviour));

		if (compIt == m_behaviours.end())
			return false;
	}

	m_behaviours.erase(compIt);
	delete behaviour;
	return true;
}

void Entity::Update()
{
	m_OnStart.Invoke();
	m_OnTick.Invoke();
}

void Entity::OnCollisionEnter(Entity* other)
{
	m_OnCollisionEnter.Invoke();
}

void Entity::OnCollisionStay(Entity* other)
{
	m_OnCollisionStay.Invoke();
}

void Entity::OnCollisionExit(Entity* other)
{
	m_OnCollisionExit.Invoke();
}

void Entity::OnTriggerEnter(Entity* other)
{
	m_OnTriggerEnter.Invoke();
}

void Entity::OnTriggerExit(Entity* other)
{
	m_OnTriggerExit.Invoke();
}

void Entity::Destroy()
{
	delete this;
}

std::vector<Behaviour*> Entity::GetAllBehaviours()
{
	std::vector<Behaviour*> behaviours;

	for (auto& [type, behaviour] : m_behaviours)
		behaviours.push_back(behaviour);

	return behaviours;
}
