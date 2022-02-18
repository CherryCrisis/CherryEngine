#include "resourceManager.hpp"

ResourceManager* ResourceManager::m_instance = nullptr;

ResourceManager* ResourceManager::GetInstance()
{
	if (!m_instance)
		m_instance = new ResourceManager();

	return m_instance;
}
