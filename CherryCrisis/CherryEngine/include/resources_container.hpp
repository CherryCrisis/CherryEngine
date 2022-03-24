#pragma once

#include <memory>
#include <string>
#include <cassert>
#include <unordered_map>
#include <type_traits>

#include <iostream>

#include "function.hpp"
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

	virtual size_t GetResourceCount() const = 0;
	virtual void GetResourcesFilepath(std::vector<const char*>& resourcePaths) const = 0;

	//Unload unused resources
	virtual void Purge() = 0;
	virtual void Remove(const char* filename) = 0;
	virtual void Erase(const char* filename) = 0;

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

	void Purge() override;
	void Remove(const char* filename) override;
	void Erase(const char* filename) override;

	size_t GetResourceCount() const override { return m_resources.size(); }
	virtual void GetResourcesFilepath(std::vector<const char*>& resourcePaths) const override;


};

#include "resources_container.inl"
