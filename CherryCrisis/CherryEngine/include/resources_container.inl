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

	auto func = CCFunction::BindFunction(&AResourcesContainer::Erase, this, resource->GetFilepath());
	resource->m_OnDeleted.Bind(&CCFunction::AFunction::Invoke, func.release());

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
	auto resources = m_resources;
	for (auto& pair : resources)
	{
		if (pair.second.use_count() <= 2)
		{
			if (pair.second->GetResourceState() != EResourceState::DESTROYED)
				pair.second->DeleteResource();
		}
	}
}

template<class ResourceT>
void ResourcesContainer<ResourceT>::Remove(const char* filename)
{
	auto pair = m_resources.find(filename);
	if (pair != m_resources.end())
	{
		std::shared_ptr<ResourceT> resource = pair->second;
		resource->DeleteResource();
	}
}

template<class ResourceT>
void ResourcesContainer<ResourceT>::Erase(const char* filename)
{
	m_resources.erase(filename);
}

template<class ResourceT>
void ResourcesContainer<ResourceT>::GetResourcesFilepath(std::vector<const char*>& resourcePaths) const
{
	for (auto& pair : m_resources)
	{
		resourcePaths.push_back(pair.second->GetFilepath());
	}
}

template<class ResourceT>
void ResourcesContainer<ResourceT>::Reload(const char* filename)
{
	auto pair = m_resources.find(filename);
	if (pair != m_resources.end())
	{
		std::shared_ptr<ResourceT> resource = pair->second;
		resource->ReloadResource();
	}
}




