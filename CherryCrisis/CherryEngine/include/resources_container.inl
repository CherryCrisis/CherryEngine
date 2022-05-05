#pragma once

template<class ResourceT>
ResourcesContainer<ResourceT>* AResourcesContainer::UnwrapResourcesContainer()
{
	assert((std::type_index)typeid(ResourceT) == m_resourceType);
	ResourcesContainer<ResourceT>* resourceContainer =  static_cast<ResourcesContainer<ResourceT>*>(this);
	return resourceContainer;

}

template<class ResourceT>
std::shared_ptr<ResourceT>* AResourcesContainer::Add(std::shared_ptr<ResourceT>& resource)
{
	ResourcesContainer<ResourceT>* resourceContainer = UnwrapResourcesContainer<ResourceT>();
	assert(resourceContainer != nullptr);

	auto func = CCFunction::BindFunctionUnsafe(&AResourcesContainer::Erase, this, (*resource->GetFilesystemPath()));
	resource->m_OnDeleted.Bind(&CCFunction::AFunction::Invoke, func.release());

	return resourceContainer->Add(resource);
}

template<class ResourceT>
std::shared_ptr<ResourceT>* AResourcesContainer::GetResource(const std::filesystem::path& filepath)
{
	return UnwrapResourcesContainer<ResourceT>()->GetResource(filepath);
}

template<class ResourceT, typename... Args>
void AResourcesContainer::Reload(const char* filename, Args... args)
{
	return UnwrapResourcesContainer<ResourceT>()->Reload(filename, args...);
}

template<class ResourceT>
std::shared_ptr<ResourceT>* ResourcesContainer<ResourceT>::Add(std::shared_ptr<ResourceT>& resource)
{
	size_t hash = std::hash<std::string>{}(resource->GetFilepath());
	auto pair = m_resources.insert({ hash, resource });

	return &pair.first->second;
}

template<class ResourceT>
std::shared_ptr<ResourceT>* ResourcesContainer<ResourceT>::ResourcesContainer::GetResource(const std::filesystem::path& filepath)
{
	size_t hash = std::hash<std::string>{}(filepath.string());
	auto resource = m_resources.find(hash);

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
			if (pair.second->GetResourceState() == EResourceState::LOADED)
				pair.second->DeleteResource();
		}
	}
}

template<class ResourceT>
void ResourcesContainer<ResourceT>::Remove(const std::filesystem::path& filepath)
{
	auto pair = m_resources.find(std::hash<std::string>{}(filepath.string()));
	if (pair != m_resources.end())
	{
		std::shared_ptr<ResourceT> resource = pair->second;
		resource->DeleteResource();
	}
}

template<class ResourceT>
void ResourcesContainer<ResourceT>::Erase(const std::filesystem::path& filepath)
{
	m_resources.erase(std::hash<std::string>{}(filepath.string()));
}

template<class ResourceT>
void ResourcesContainer<ResourceT>::Rename(const std::filesystem::path& filepath, const char* newFilepath)
{
	std::shared_ptr<ResourceT> resource = *GetResource(filepath);
	resource->SetFilepath(newFilepath);

	Erase(filepath);
	Add(resource);
}

template<class ResourceT>
void ResourcesContainer<ResourceT>::GetResourcesFilepath(std::vector<std::filesystem::path*>& resourcePaths) const
{
	for (auto& pair : m_resources)
	{
		resourcePaths.push_back(pair.second->GetFilesystemPath());
	}
}

template<class ResourceT>
template<class... Args>
void ResourcesContainer<ResourceT>::Reload(const std::filesystem::path& filepath, Args... args)
{
	auto pair = m_resources.find(std::hash<std::string>{}(filepath.generic_string()));
	if (pair != m_resources.end())
	{
		std::shared_ptr<ResourceT> resource = pair->second;
		Resource<ResourceT>::ReloadResource(pair->second, args...);
	}
}




