#pragma once

template<class ResourceT>
ResourcesContainer<ResourceT>* AResourcesContainer::UnwrapResourcesContainer()
{
	assert((std::type_index)typeid(ResourceT) == m_resourceType);
	ResourcesContainer<ResourceT>* resourceContainer =  static_cast<ResourcesContainer<ResourceT>*>(this);
	return resourceContainer;

}

template<class ResourceT>
void AResourcesContainer::Add(const char* filename, std::shared_ptr<ResourceT>& resource)
{
	ResourcesContainer<ResourceT>* resourceContainer = UnwrapResourcesContainer<ResourceT>();
	assert(resourceContainer != nullptr);

	resourceContainer->Add(filename, resource);
}

template<class ResourceT>
std::shared_ptr<ResourceT>* AResourcesContainer::GetResource(const char* filename)
{
	return UnwrapResourcesContainer<ResourceT>()->GetResource(filename);
}

template<class ResourceT>
void ResourcesContainer<ResourceT>::Add(const char* filename, std::shared_ptr<ResourceT>& resource)
{
	m_resources.emplace(filename, std::shared_ptr<ResourceT>(resource));
}

template<class ResourceT>
std::shared_ptr<ResourceT>* ResourcesContainer<ResourceT>::ResourcesContainer::GetResource(const char* filename)
{
	auto resource = m_resources.find(filename);

	if (resource != m_resources.end())
	{
		return &resource->second;
	}

	return nullptr;
}

template<class ResourceT>
void ResourcesContainer<ResourceT>::Purge()
{
	for (auto& pair : m_resources)
	{
		if (pair.second.use_count() <= 1)
		{
			std::cout << pair.second->GetFilepath() << " " << std::to_string(pair.second.use_count()) << std::endl;
			delete pair.second.get();
			m_resources.erase(pair.first);
		}
	}
}

template<class ResourceT>
void ResourcesContainer<ResourceT>::Remove(const char* filename)
{
	auto pair = m_resources.find(filename);
	if (pair != m_resources.end())
	{
		std::cout << pair->second->GetFilepath() << " deleted " << std::endl;
		pair->second.reset();
		m_resources.erase(pair->first);
	}
}


