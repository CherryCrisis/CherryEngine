#pragma once

template <class T, typename... Args>
std::shared_ptr<T> ResourceManager::AddResource(const char* filepath, bool verifIsExist, Args... args)
{
	//LockGuard lock(lockRM);

	if (verifIsExist)
	{
		std::shared_ptr<T> findedResource = GetResource<T>(filepath);
		if (findedResource != nullptr)
			return findedResource;
	}

	Resource::Ref<T> resourcePtr = std::make_shared<T>(filepath);

	auto resourceMap = std::make_pair<std::type_index, std::shared_ptr<Resource>>(typeid(T), resourcePtr);

	auto lockMap = m_resourceLocks.find(typeid(T));
	std::shared_ptr<std::mutex> mutex = nullptr;

	if (lockMap == m_resourceLocks.end())
	{
		auto resourceLockMap = m_resourceLocks.emplace(typeid(T), std::make_shared<std::mutex>());
		mutex = resourceLockMap.first->second;
	}
	else
	{
		mutex = lockMap->second;
	}

	{
		LockGuard lockGuard(mutex);
		m_resources.insert(resourceMap);
	}

	if (!resourcePtr)
		return nullptr;

	T::Load(resourcePtr, filepath, args...);

	return resourcePtr;
}

template<class T, class CallbackType, typename... Args>
void ResourceManager::AddResourceWithCallback(const char* filepath, bool verifIsExist,
	CCCallback::AWrapCallback* wrappedCallback, Args... args)
{
	Resource::Ref<T> resourceRef = AddResource<T>(filepath, verifIsExist, std::forward<Args>(args)...);
	std::shared_ptr<Resource> resource = std::dynamic_pointer_cast<Resource>(resourceRef);

	auto unwrapedCallback = static_cast<CCCallback::ACallback<CallbackType>*>(wrappedCallback);
	std::unique_ptr<CCCallback::ACallback<CallbackType>> uniqueCallback(unwrapedCallback);

	resource->m_onLoaded.Bind(uniqueCallback, uniqueCallback->m_typeIndex);

	auto eventFunction = CCFunction::BindFunction(&Resource::IsLoaded, resource.get(), resource);
	threadpool->CreateTask(eventFunction, EChannelTask::MainThread);
}

template<class T, class CallbackType, typename... Args>
void ResourceManager::AddResourceMultiThreads(const char* filepath, bool verifIsExist,
	std::unique_ptr<CCCallback::ACallback<CallbackType>>& uniqueCallback, Args&&... args)
{
	CCCallback::AWrapCallback* wrappedCallback(static_cast<CCCallback::AWrapCallback*>(uniqueCallback.release()));

	auto function = 
		CCFunction::BindFunction(
			&ResourceManager::AddResourceWithCallback<T, CallbackType, Args...>, this,
			filepath, verifIsExist, wrappedCallback, args...);

	threadpool->CreateTask(function, EChannelTask::Multithread);
}

template<class T>
std::shared_ptr<T> ResourceManager::GetResource(const char* filepath)
{
	auto lockMap = m_resourceLocks.find(typeid(T));
	if (lockMap != m_resourceLocks.end())
	{
		LockGuard lockGuard(lockMap->second);

		auto resourceRange = m_resources.equal_range(typeid(T));
		const size_t hashKey = std::hash<std::string>()(filepath);

		for (auto& it = resourceRange.first; it != resourceRange.second; ++it)
		{
			if (it->second->GetHashId() == hashKey)
				return (std::dynamic_pointer_cast<T>(it->second));
		}
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


