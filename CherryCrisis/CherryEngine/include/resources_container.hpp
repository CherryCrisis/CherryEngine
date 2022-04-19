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
	void Add(std::shared_ptr<ResourceT>& resource);

	template<class ResourceT>
	std::shared_ptr<ResourceT>* GetResource(const std::filesystem::path& filepath);

	virtual size_t GetResourceCount() const = 0;
	virtual void GetResourcesFilepath(std::vector<std::filesystem::path*>& resourcePaths) const = 0;

	//Unload unused resources
	virtual void Purge() = 0;
	virtual void Remove(const std::filesystem::path& filepath) = 0;
	virtual void Erase(const std::filesystem::path& filepath) = 0;
	
	template<class ResourceT, typename... Args>
	void Reload(const char* filename, Args... args);

};

template<class ResourceT>
class ResourcesContainer : public AResourcesContainer
{
protected:
	std::unordered_map<size_t, std::shared_ptr<ResourceT>> m_resources;
public:

	ResourcesContainer()
		: AResourcesContainer(typeid(ResourceT)) {}

	void Add(std::shared_ptr<ResourceT>& resource);
	std::shared_ptr<ResourceT>* GetResource(const std::filesystem::path& filename);

	void Purge() override;
	void Remove(const std::filesystem::path& filename) override;
	void Erase(const std::filesystem::path& filename) override;

	template<class... Args>
	void Reload(const std::filesystem::path& filepath, Args... args);

	size_t GetResourceCount() const override { return m_resources.size(); }
	virtual void GetResourcesFilepath(std::vector<std::filesystem::path*>& resourcePaths) const override;


};

#include "resources_container.inl"
