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

	size_t resourceTypeSize = m_resources.size();

	for (int i = 0; i < resourceTypeSize; ++i)
	{
		for (auto& pair : m_resources)
		{
			pair.second->Purge();
		}
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

void ResourceManager::GetResourcesPath(std::map<std::type_index, std::vector<std::filesystem::path*>>& resourcePaths) const
{
	for (auto& pair : m_resources)
	{
		std::vector<std::filesystem::path*> filepaths;
		pair.second->GetResourcesFilepath(filepaths);

		resourcePaths.emplace(pair.first, filepaths);
	}
}

void ResourceManager::Rename(const std::type_index& typeID, const std::filesystem::path& filepath, const char* newFilepath)
{
	auto resourceContainerIt = m_resources.find(typeID);
	if (resourceContainerIt != m_resources.end())
	{
		resourceContainerIt->second->Rename(filepath, newFilepath);
	}
}
