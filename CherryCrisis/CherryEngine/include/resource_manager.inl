#pragma once

template <class T, typename... Args>
std::shared_ptr<T> ResourceManager::AddResource(const char* filepath, bool verifIsExist, Args... args)
{
	if (verifIsExist)
	{
		std::shared_ptr<T> findedResource = GetResource<T>(filepath);
		if (findedResource != nullptr)
			return findedResource;
	}

	Resource::Ref<T> resourcePtr = T::Create(filepath, args...);

	if (!resourcePtr)
		return nullptr;

	auto resourceMap = std::make_pair<std::type_index, std::shared_ptr<Resource>>(typeid(T), resourcePtr);

	m_resources.insert(resourceMap);

	//resourcePtr->m_onLoaded->Invoke(resourcePtr);

	return resourcePtr;
}

template<class T, class CallbackType, typename... Args>
void ResourceManager::AddResourceMultiThreads(const char* filepath, bool verifIsExist,
	std::unique_ptr<CCCallback::ACallback<CallbackType>>& callback, Args... args)
{
	/*std::unique_ptr<CCFunction::AFunction> function = CCFunction::BindFunction(&ResourceManager::TestResource<T, CallbackType, Args...>, this,
		filepath, verifIsExist, std::move(callback), args...);*/

	std::unique_ptr<CCFunction::AFunction> function = CCFunction::BindFunction(&ResourceManager::AddResource<T, Args...>, this,
		join<filepath, verifIsExist, args...>);

	threadpool->CreateTask(function, EChannelTask::Multithread);
}

template<class T>
std::shared_ptr<T> ResourceManager::GetResource(const char* filepath) const
{
	auto resourceRange = m_resources.equal_range(typeid(T));
	
	const size_t hashKey = std::hash<std::string>()(filepath);
	
	for (auto& it = resourceRange.first; it != resourceRange.second; ++it)
	{
		if (it->second->GetHashId() == hashKey)
			return (std::dynamic_pointer_cast<T>(it->second));
	}

	return nullptr;
}

template<class T>
void ResourceManager::GetAllResources(std::vector<std::shared_ptr<T>>& resources) const
{
	auto resourceRange = m_resources.equal_range(typeid(T));

	auto& firstIt = resourceRange.first;
	auto& lastIt = resourceRange.second;

	resources.reserve(size_t(lastIt - firstIt));

	for (auto& it = firstIt; it != lastIt; ++it)
	{
		resources.push_back(std::dynamic_pointer_cast<T>(it->second));
	}
}


