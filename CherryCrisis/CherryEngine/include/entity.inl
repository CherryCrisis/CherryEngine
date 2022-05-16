#pragma once	

#include "utils.hpp"

template <BehaviourT CompT>
CompT* Entity::AddBehaviour()
{
	CompT* rawPtr = new CompT();
	rawPtr->m_owner = this;

	m_behaviours.insert({ String::ExtractTypeName<CompT>(), rawPtr });
	rawPtr->BindToSignals();

	return rawPtr;
}

template <BehaviourT CompT>
CompT* Entity::GetBehaviour()
{
	auto compIt = m_behaviours.find(String::ExtractTypeName<CompT>());

	if (compIt != m_behaviours.end())
		return static_cast<CompT*>(compIt->second);

	auto itPair = m_behaviours.equal_range("Behaviour");

	for (auto findIt = itPair.first; findIt != itPair.second; findIt++)
	{
		auto castedComp = dynamic_cast<CompT*>(findIt->second);

		if (castedComp)
			return castedComp;
	}

	return nullptr;
}

template <BehaviourT CompT>
std::vector<CompT*> Entity::GetBehavioursOfType()
{
	std::vector<CompT*> behaviours;

	auto itPair = m_behaviours.equal_range(String::ExtractTypeName<CompT>());

	for (auto compIt = itPair.first; compIt != itPair.second; compIt++)
		behaviours.push_back(static_cast<CompT*>(compIt->second));

	return behaviours;
}

template <BehaviourT CompT>
bool Entity::HasBehaviour()
{
	return GetBehaviour<CompT>() != nullptr;
}

template <BehaviourT CompT>
bool Entity::TryGetBehaviour(CompT*& componentToReturn)
{
	componentToReturn = GetBehaviour<CompT>();

	return componentToReturn != nullptr;
}

template <BehaviourT CompT>
CompT* Entity::GetOrAddBehaviour()
{
	CompT* componentToReturn = nullptr;
	if (TryGetBehaviour(componentToReturn))
		return componentToReturn;

	return AddBehaviour<CompT>();
}

template <BehaviourT CompT>
void Entity::SubscribeComponent(CompT* behaviour)
{
	SubscribeComponent(behaviour, String::ExtractTypeName<CompT>());
}
