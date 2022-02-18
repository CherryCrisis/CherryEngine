#pragma once

#include "resourceManager.hpp"

#include "mesh.hpp"

template <class T, typename... Args>
std::shared_ptr<T> ResourceManager::AddResource(const char* filepath, bool verifIsExist, Args... args)
{
	if (verifIsExist)
	{
		std::shared_ptr<T> findedResource = GetResource<T>(filepath);
		if (findedResource.get() != nullptr)
			return findedResource;
	}

	//TODO : Change this !
	Resource* resource = T::Create(filepath, args...);

	if (resource == nullptr)
		return nullptr;

	auto resourceMap = std::make_pair<std::type_index, std::shared_ptr<Resource>>
		(typeid(T), std::shared_ptr<Resource>(resource));

	m_resources.insert(resourceMap);

	return std::dynamic_pointer_cast<T>(resourceMap.second);
}

template<class T>
std::shared_ptr<T> ResourceManager::GetResource(const char* filepath) const
{
	auto resourceRange = m_resources.equal_range(typeid(T));

	const size_t hashKey = std::hash<std::string>()(filepath);

	for (auto it = resourceRange.first; it != resourceRange.second; ++it)
	{
		if (it->second->GetHashId() == hashKey)
			return (std::dynamic_pointer_cast<T>(it->second));
	}

	return nullptr;
}

template<class T>
void ResourceManager::GetAllResources(std::vector<std::shared_ptr<T>>& resources) const
{
	auto resourceRange = m_resources.equal_range(typeid(T));

	auto firstIt = resourceRange.first;
	auto lastIt = resourceRange.second;

	resources.reserve(size_t(lastIt - firstIt));

	for (auto it = firstIt; it != lastIt; ++it)
	{
		resources.push_back(std::dynamic_pointer_cast<T>(it->second));
	}
}


