#pragma once	

template <class CompT>
CompT* Entity::AddBehaviour()
{
	static_assert(!std::is_same_v<Behaviour, CompT>, "CompT cannot be a pure Behaviour");
	static_assert(std::is_base_of_v<Behaviour, CompT>, "CompT is not inherited of Behaviour");

	CompT* rawPtr = new CompT();
	rawPtr->m_owner = this;

	m_behaviours.insert({ typeid(CompT), rawPtr });
	rawPtr->BindToSignals();

	return rawPtr;
}

template <class CompT>
CompT* Entity::GetBehaviour()
{
	static_assert(!std::is_same_v<Behaviour, CompT>, "CompT cannot be a pure Behaviour");
	static_assert(std::is_base_of_v<Behaviour, CompT>, "CompT is not inherited of Behaviour");

	auto compIt = m_behaviours.find(typeid(CompT));

	if (compIt != m_behaviours.end())
		return static_cast<CompT*>(compIt->second);

	auto itPair = m_behaviours.equal_range(typeid(Behaviour));

	for (auto findIt = itPair.first; findIt != itPair.second; findIt++)
	{
		auto castedComp = dynamic_cast<CompT*>(findIt->second);

		if (castedComp)
			return castedComp;
	}

	return nullptr;
}

template <class CompT>
std::vector<CompT*> Entity::GetBehaviours()
{
	static_assert(!std::is_same_v<Behaviour, CompT>, "CompT cannot be a pure Behaviour");
	static_assert(std::is_base_of_v<Behaviour, CompT>, "CompT is not inherited of Behaviour");

	std::vector<CompT*> behaviours;

	auto itPair = m_behaviours.equal_range(typeid(CompT));

	for (auto compIt = itPair.first; compIt != itPair.second; compIt++)
		behaviours.push_back(static_cast<CompT*>(compIt->second));

	return behaviours;
}

template <class CompT>
bool Entity::HasBehaviour()
{
	static_assert(!std::is_same_v<Behaviour, CompT>, "CompT cannot be a pure Behaviour");
	static_assert(std::is_base_of_v<Behaviour, CompT>, "CompT is not inherited of Behaviour");

	return GetBehaviour<CompT>() != nullptr;
}

template <class CompT>
bool Entity::TryGetBehaviour(CompT*& componentToReturn)
{
	static_assert(!std::is_same_v<Behaviour, CompT>, "CompT cannot be a pure Behaviour");
	static_assert(std::is_base_of_v<Behaviour, CompT>, "CompT is not inherited of Behaviour");
	
	componentToReturn = GetBehaviour<CompT>();

	return componentToReturn != nullptr;
}

template <class CompT>
CompT* Entity::GetOrAddBehaviour()
{
	static_assert(!std::is_same_v<Behaviour, CompT>, "CompT cannot be a pure Behaviour");
	static_assert(std::is_base_of_v<Behaviour, CompT>, "CompT is not inherited of Behaviour");

	CompT* componentToReturn = nullptr;
	if (TryGetBehaviour(componentToReturn))
		return componentToReturn;

	return AddBehaviour<CompT>();
}

template <class CompT>
void Entity::SubscribeComponent(CompT* behaviour)
{
	behaviour->m_owner = this;
	behaviour->BindToSignals();
	m_behaviours.insert({ typeid(CompT), behaviour });
}