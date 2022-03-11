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
	: threadpool(ThreadPool::GetInstance())
{
}

ResourceManager::~ResourceManager()
{

}
