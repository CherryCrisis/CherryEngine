#include "pch.hpp"

#include "resource_manager.hpp"

template<>
ResourceManager* Singleton<ResourceManager>::currentInstance = nullptr;

ResourceManager::ResourceManager()
	: m_threadpool(ThreadPool::GetInstance()), m_lockResources()
{
}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::Purge()
{
	std::lock_guard<std::mutex> lock(m_lockResources);

	for (auto& pair : m_resources)
	{
		pair.second->Purge();
	}
}

size_t ResourceManager::GetResourceCount() const
{
	size_t resourceCount = 0;
	for (auto& pair : m_resources)
	{
		resourceCount += pair.second->GetResourceCount();
	}

	return resourceCount;
}

void ResourceManager::GetResourcesPath(std::map<std::type_index, std::vector<const char*>>& resourcePaths) const
{
	/*for (auto& pair : m_resources)
	{
		std::vector<const char*> filepaths;
		pair.second->GetResourcesFilepath(filepaths);

		resourcePaths.emplace(pair.first, filepaths);
	}*/
}
