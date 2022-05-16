#include "pch.hpp"

#include "entity.hpp"

#include "debug.hpp"

#include "behaviour.hpp"
#include "transform.hpp"
#include "scene.hpp"

#include <typeinfo>
Entity::Entity()
{

}

Entity::Entity(const std::string& name, CCUUID id)
	: Object(id), m_name(name)
{
}


Entity::~Entity()
{
	for (auto& [type, behaviour] : m_behaviours)
		delete behaviour;

	m_cell->RemoveEntity(this);
}

void Entity::Initialize(Scene* scene) 
{
	if (scene)
		scene->AddEntityToDefault(this);

	for (auto& [type, behaviour] : m_behaviours)
		behaviour->BindToSignals();
	
	m_OnAwake.Invoke();	
}	

void Entity::Initialize(Scene* scene, const std::string& cellName)
{
	if (scene)
		scene->AddEntityToCell(this, cellName);

	for (auto& [type, behaviour] : m_behaviours)
		behaviour->BindToSignals();

	m_OnAwake.Invoke();
}

bool Entity::RemoveBehaviour(Behaviour* behaviour)
{
	auto compIt = m_behaviours.find(typeid(*behaviour));

	if (compIt == m_behaviours.end())
	{
		auto itPair = m_behaviours.equal_range(typeid(Behaviour));

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

std::vector<Behaviour*> Entity::GetAllBehaviours()
{
	std::vector<Behaviour*> behaviours;

	for (auto& [type, behaviour] : m_behaviours)
		behaviours.push_back(behaviour);

	return behaviours;
}
