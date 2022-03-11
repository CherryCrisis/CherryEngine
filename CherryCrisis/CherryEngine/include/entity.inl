#pragma once	

template <class CompT>
CompT* Entity::AddBehaviour()
{
	static_assert(!std::is_same_v<Behaviour, CompT>, "CompT cannot be a pure Behaviour");
	static_assert(std::is_base_of_v<Behaviour, CompT>, "CompT is not inherited of Behaviour");

	CompT* rawPtr = new CompT(*this);
	m_behaviours.push_back(rawPtr);

	return rawPtr;
}

template <class CompT>
CompT* Entity::GetBehaviour()
{
	static_assert(!std::is_same_v<Behaviour, CompT>, "CompT cannot be a pure Behaviour");
	static_assert(std::is_base_of_v<Behaviour, CompT>, "CompT is not inherited of Behaviour");

	for (Behaviour* behaviour : m_behaviours)
	{
		CompT* castedComponent = dynamic_cast<CompT*>(behaviour);

		if (castedComponent)
			return castedComponent;
	}

	return nullptr;
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