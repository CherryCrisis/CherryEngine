#pragma once

template<class T>
std::shared_ptr<T> ResourceManager::CreateResource(const char* filepath)
{
	std::shared_ptr<T> resourcePtr = std::make_shared<T>(filepath);

	auto resourceContainerIt = m_resources.find(typeid(T));
	if (resourceContainerIt == m_resources.end())
	{
		auto pair = m_resources.emplace(typeid(T), new ResourcesContainer<T>());
		pair.first->second->Add(filepath, resourcePtr);
	}
	else
	{
		resourceContainerIt->second->Add(filepath, resourcePtr);
	}


	return resourcePtr;
}

template <class T, typename... Args>
std::shared_ptr<T> ResourceManager::AddResource(const char* filepath, bool verifIsExist, Args... args)
{
	std::shared_ptr<T> resourcePtr;

	{
		std::lock_guard<std::mutex> lock(m_lockResources);

		if (verifIsExist)
		{
			auto resourceContainerIt = m_resources.find(typeid(T));
			if (resourceContainerIt != m_resources.end())
			{
				std::shared_ptr<T>* findedResource = resourceContainerIt->second->GetResource<T>(filepath);
				if (findedResource != nullptr)
					return *findedResource;
			}
		}

		resourcePtr = CreateResource<T>(filepath);
	}

	T::Load(resourcePtr, args...);

	resourcePtr->IsLoaded(resourcePtr, threadpool);

	return resourcePtr;
}

template<class T, class CallbackType, typename... Args>
void ResourceManager::AddResourceWithCallback(const char* filepath, bool verifIsExist,
	CCCallback::AWrapCallback* wrappedCallback, Args... args)
{
	std::shared_ptr<T> resourcePtr;

	auto unwrapedCallback = static_cast<CCCallback::ACallback<CallbackType>*>(wrappedCallback);
	std::unique_ptr<CCCallback::ACallback<CallbackType>> uniqueCallback(unwrapedCallback);

	{
		std::lock_guard<std::mutex> lock(m_lockResources);

		if (verifIsExist)
		{
			auto resourceContainerIt = m_resources.find(typeid(T));
			if (resourceContainerIt != m_resources.end())
			{
				std::shared_ptr<T>* findedResource = resourceContainerIt->second->GetResource<T>(filepath);
				if (findedResource != nullptr)
				{
					if ((*findedResource)->GetIsLoaded())
					{
						auto function = CCFunction::BindFunction(&CCCallback::AWrapCallback::Invoke,
							wrappedCallback, (*findedResource));

						threadpool->CreateTask(function, EChannelTask::MAINTHREAD);
					}
				}

				(*findedResource)->m_onLoaded.Bind(uniqueCallback);
				return;
			}

		}

		resourcePtr = CreateResource<T>(filepath);
	}


	T::Load(resourcePtr, args...);

	resourcePtr->m_onLoaded.Bind(uniqueCallback);
	resourcePtr->IsLoaded(resourcePtr, threadpool);
}

template<class T, class CallbackType, typename... Args>
void ResourceManager::AddResourceMultiThreads(const char* filepath, bool verifIsExist,
	std::unique_ptr<CCCallback::ACallback<CallbackType>>& uniqueCallback, Args&&... args)
{
	//Wrap callback because CCFunction::BindFunction(...) doesn't work with an arg<Type<OtherType>>
	CCCallback::AWrapCallback* wrappedCallback(static_cast<CCCallback::AWrapCallback*>(uniqueCallback.release()));

	auto function = 
		CCFunction::BindFunctionUnsafe(
			&ResourceManager::AddResourceWithCallback<T, CallbackType, Args...>, this,
			filepath, verifIsExist, wrappedCallback, args...);

	threadpool->CreateTask(function, EChannelTask::MULTITHREAD);
}

//template<class T>
//std::shared_ptr<T> ResourceManager::GetResource(const char* filepath)
//{
//	std::lock_guard<std::mutex> lock(m_lockResources);
//
//	const size_t hashKey = std::hash<std::string>()(filepath);
//	auto resourceRange = m_resources.equal_range(typeid(T));
//
//	for (auto& it = resourceRange.first; it != resourceRange.second; ++it)
//	{
//		if (it->second->GetHashId() == hashKey)
//		{
//			return (std::dynamic_pointer_cast<T>(it->second));
//		}
//	}
//
//	return nullptr;
//}
//
//template<class T>
//void ResourceManager::GetAllResources(std::vector<std::shared_ptr<T>>& resources) const
//{
//	std::lock_guard<std::mutex> lock(m_lockResources);
//
//	auto resourceRange = m_resources.equal_range(typeid(T));
//
//	auto& firstIt = resourceRange.first;
//	auto& lastIt = resourceRange.second;
//
//	resources.reserve(size_t(lastIt - firstIt));
//
//	for (auto& it = firstIt; it != lastIt; ++it)
//	{
//		resources.push_back(std::dynamic_pointer_cast<T>(it->second));
//	}
//}


