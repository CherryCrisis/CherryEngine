#pragma once

#include "resourceManager.hpp"

#include "mesh.hpp"

//template<class T, typename... Args>
//inline std::shared_ptr<T> ResourceManager::AddResource(const char* filepath, Args... args)
//{
//	return AddResource<T>(filepath, true, args);
//}

template <class T, typename... Args>
inline std::shared_ptr<T> ResourceManager::AddResource(const char* filepath, bool verifIsExist, Args... args)
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
inline std::shared_ptr<T> ResourceManager::GetResource(const char* filepath) const
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
inline void ResourceManager::GetAllResources(std::vector<std::shared_ptr<T>>& resources) const
{
	auto resourceRange = m_resources.equal_range(typeid(T));

	for (auto it = resourceRange.first; it != resourceRange.second; ++it)
	{
		resources.push_back(std::dynamic_pointer_cast<T>(it->second));
	}
}


