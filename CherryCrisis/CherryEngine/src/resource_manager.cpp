#include "pch.hpp"

#include "resource_manager.hpp"

ResourceManager* ResourceManager::m_instance = nullptr;

ResourceManager* ResourceManager::GetInstance()
{
	if (!m_instance)
		m_instance = new ResourceManager();

	return m_instance;
}

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
