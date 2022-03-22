#pragma once

#include <set>
#include <memory>
#include <string>
#include <cassert>
#include <unordered_map>
#include <optional>

#include "resource.hpp"

template<class ResourceT>
class ResourcesContainer;

class AResourcesContainer
{
public:
	std::type_index m_resourceType;

	template<class ResourceT>
	ResourcesContainer<ResourceT>* UnwrapResourcesContainer();

public:
	AResourcesContainer(std::type_index resourceType)
		: m_resourceType(resourceType) {}

	virtual ~AResourcesContainer() = default;
	
	template<class ResourceT>
	void Add(const char* filename, std::shared_ptr<ResourceT>& resource);

	template<class ResourceT>
	std::shared_ptr<ResourceT>* GetResource(const char* filename);
};

template<class ResourceT>
class ResourcesContainer : public AResourcesContainer
{
protected:
	std::unordered_map<std::string, std::shared_ptr<ResourceT>> m_resources;

public:
	ResourcesContainer() 
		: AResourcesContainer(typeid(ResourceT)) {}

	void Add(const char* filename, std::shared_ptr<ResourceT>& resource);
	std::shared_ptr<ResourceT>* GetResource(const char* filename);

};

#include "resources_container.inl"
