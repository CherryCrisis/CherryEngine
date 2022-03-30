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

template<class T>
std::shared_ptr<T> ResourceManager::AddResourceRef(const char* filepath, bool verifIsExist)
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

	return resourcePtr;
}

template <class T, typename... Args>
std::shared_ptr<T> ResourceManager::AddResource(const char* filepath, bool verifIsExist, Args... args)
{
	std::shared_ptr<T> resourcePtr = AddResourceRef<T>(filepath, verifIsExist);
	EResourceState resourceState = resourcePtr->GetResourceState();

	switch (resourceState)
	{
	case EResourceState::EMPTY:
		resourcePtr->SetResourceState(EResourceState::LOADING);/*666*/
		T::Load(resourcePtr, args...);
		resourcePtr->IsLoaded(resourcePtr, m_threadpool);
		break;
	default:
		break;
	}

	return resourcePtr;
}

template<class T, class CallbackType, typename... Args>
void ResourceManager::AddResourceWithCallback(const char* filepath, bool verifIsExist,
	CCCallback::AWrapCallback* wrappedCallback, Args... args)
{
	std::shared_ptr<T> resourcePtr = AddResourceRef<T>(filepath, verifIsExist);

	auto unwrapedCallback = static_cast<CCCallback::ACallback<CallbackType>*>(wrappedCallback);
	std::unique_ptr<CCCallback::ACallback<CallbackType>> uniqueCallback(unwrapedCallback);

	EResourceState resourceState = resourcePtr->GetResourceState();

	switch (resourceState)
	{
	case EResourceState::EMPTY:
		resourcePtr->SetResourceState(EResourceState::LOADING);/*666*/
		T::Load(resourcePtr, args...);
		resourcePtr->m_OnLoaded.Bind(uniqueCallback);
		resourcePtr->IsLoaded(resourcePtr, m_threadpool);
		break;
	case EResourceState::LOADING:
		resourcePtr->m_OnLoaded.Bind(uniqueCallback);
		break;
	case EResourceState::LOADED:
	{
		/*std::unique_ptr<CCFunction::AFunction> function = CCFunction::BindFunction(&CCCallback::AWrapCallback::Invoke,
			wrappedCallback, resourcePtr);

		m_threadpool->CreateTask(function, EChannelTask::MAINTHREAD);
		resourcePtr->m_OnLoaded.Bind(uniqueCallback);*/
	}
	break;

	default:
		break;
	}
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

	m_threadpool->CreateTask(function, EChannelTask::MULTITHREAD);
}

template<class T>
std::shared_ptr<T> ResourceManager::GetResource(const char* filepath)
{
	std::lock_guard<std::mutex> lock(m_lockResources);

	auto resourceContainerIt = m_resources.find(typeid(T));
	if (resourceContainerIt != m_resources.end())
	{
		std::shared_ptr<T>* findedResource = resourceContainerIt->second->GetResource<T>(filepath);
		if (findedResource != nullptr)
			return *findedResource;
	}

	return nullptr;
}

template<class T>
void ResourceManager::Remove(const char* filepath)
{
	std::lock_guard<std::mutex> lock(m_lockResources);

	auto resourceContainerIt = m_resources.find(typeid(T));
	if (resourceContainerIt != m_resources.end())
	{
		resourceContainerIt->second->Remove(filepath);
	}
}

template<class T>
void ResourceManager::Reload(const char* filepath)
{
	std::lock_guard<std::mutex> lock(m_lockResources);

	auto resourceContainerIt = m_resources.find(typeid(T));
	if (resourceContainerIt != m_resources.end())
	{
		resourceContainerIt->second->Reload(filepath);
	}
}


