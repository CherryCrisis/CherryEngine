#pragma once	

#include "utils.hpp"

template <BehaviourT CompT>
CompT* Entity::AddBehaviour()
{
	return AddBehaviour<CompT>(String::ExtractTypeName<CompT>());
}

template <BehaviourT CompT>
CompT* Entity::AddBehaviour(const std::string& componentTypeName)
{
	CompT* rawPtr = new CompT();

	SubscribeComponent(rawPtr, componentTypeName);

	return rawPtr;
}


template <BehaviourT CompT>
CompT* Entity::GetBehaviour()
{
	return static_cast<CompT*>(GetBehaviour(String::ExtractTypeName<CompT>()));
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
