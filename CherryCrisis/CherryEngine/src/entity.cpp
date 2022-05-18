#include "pch.hpp"

#include "entity.hpp"

#include <typeinfo>

#include "cell_system.hpp"

#include "behaviour.hpp"

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
	auto compIt = m_behaviours.find(String::ExtractTypeIndexName(typeid(*behaviour)));

	if (compIt == m_behaviours.end())
	{
		auto itPair = m_behaviours.equal_range("Behaviour");

		for (auto findIt = itPair.first; findIt != itPair.second; findIt++)
		{
			if (findIt->second == behaviour)
			{
				compIt = findIt;
				break;
			}
		}

		if (compIt == m_behaviours.end())
			return false;
	}

	m_behaviours.erase(compIt);
	delete behaviour;
	return true;
}

void Entity::Update()
{
	m_OnAwake.Invoke();
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
	m_OnDestroyed.Invoke();
	delete this;
}

Behaviour* Entity::GetBehaviour(const std::string& componentTypeName)
{
	auto compIt = m_behaviours.find(componentTypeName);

	if (compIt != m_behaviours.end())
		return compIt->second;

	auto itPair = m_behaviours.equal_range("Behaviour");

	for (auto findIt = itPair.first; findIt != itPair.second; findIt++)
	{
		std::string behaviourTypeName = String::ExtractTypeIndexName(typeid(*findIt->second));

		if (componentTypeName == behaviourTypeName)
			return findIt->second;
	}

	return nullptr;
}

std::vector<Behaviour*> Entity::GetAllBehaviours()
{
	std::vector<Behaviour*> behaviours;

	for (auto& [type, behaviour] : m_behaviours)
		behaviours.push_back(behaviour);

	return behaviours;
}

void Entity::SubscribeComponent(Behaviour* behaviour, const std::string& componentTypeName)
{
	behaviour->m_owner = this;
	m_behaviours.insert({ componentTypeName, behaviour });
}

std::vector<Behaviour*> Entity::GetBehavioursOfType(const std::string& componentTypeName)
{
	std::vector<Behaviour*> behaviours;

	auto itPair = m_behaviours.equal_range(componentTypeName);

	for (auto compIt = itPair.first; compIt != itPair.second; compIt++)
		behaviours.push_back(compIt->second);

	return behaviours;
}
