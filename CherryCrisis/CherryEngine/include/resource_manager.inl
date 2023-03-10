#pragma once

template<class T>
std::shared_ptr<T>& ResourceManager::CreateResource(const char* filepath)
{
	std::filesystem::path normalPath = std::filesystem::path(filepath).lexically_normal();
	std::string normalPathStr = normalPath.string();

	std::shared_ptr<T> resource = std::make_shared<T>(normalPathStr.c_str());

	auto resourceContainerIt = m_resources.find(typeid(T));

	std::shared_ptr<T>* resourcePtr;
	if (resourceContainerIt == m_resources.end())
	{
		auto pair = m_resources.emplace(typeid(T), new ResourcesContainer<T>());
		resourcePtr = pair.first->second->Add(resource);
	}
	else
	{
		resourcePtr = resourceContainerIt->second->Add(resource);
	}

	return *resourcePtr;
}

template<class T>
std::shared_ptr<T>& ResourceManager::AddResourceRef(const char* filepath, bool verifIsExist)
{

	std::lock_guard<std::mutex> lock(m_lockResources);

	if (verifIsExist)
	{
		auto resourceContainerIt = m_resources.find(typeid(T));
		if (resourceContainerIt != m_resources.end())
		{
			std::filesystem::path normalPath = std::filesystem::path(filepath).lexically_normal();
			std::string normalPathStr = normalPath.string();

			std::shared_ptr<T>* findedResource = resourceContainerIt->second->GetResource<T>(normalPathStr.c_str());
			if (findedResource != nullptr)
				return *findedResource;
		}
	}

	std::shared_ptr<T>& resourceRef = CreateResource<T>(filepath);

	return resourceRef;
}

template <class T, typename... Args>
std::shared_ptr<T> ResourceManager::AddResource(const char* filepath, bool verifIsExist, Args... args)
{
	std::shared_ptr<T> resourcePtr = AddResourceRef<T>(filepath, verifIsExist);

	if (EResourceState::EMPTY == resourcePtr->GetResourceState())
	{
		resourcePtr->SetResourceState(EResourceState::LOADING);
		T::Load(resourcePtr, args...);
		resourcePtr->IsLoaded(resourcePtr, m_threadpool);
	}

	return resourcePtr;
}

template<class T, typename... Args>
void ResourceManager::AddResourceWithCallback(std::shared_ptr<T>& resource,
	std::shared_ptr<CCCallback::AWrapCallback> wrappedCallback, Args... args)
{
	auto callback = std::dynamic_pointer_cast<CCCallback::ACallback<std::shared_ptr<T>>>(wrappedCallback);

	EResourceState resourceState = resource->GetResourceState();

	switch (resourceState)
	{
	case EResourceState::EMPTY:
		resource->SetResourceState(EResourceState::LOADING);
		T::Load(resource, args...);

		if (callback)
			resource->m_OnLoaded.Bind(callback);
		resource->IsLoaded(resource, m_threadpool);
		break;
	case EResourceState::LOADING:
		if (callback)
			resource->m_OnLoaded.Bind(callback);
		break;
	case EResourceState::LOADED:
	{
		if (!callback)
			return;

		resource->m_OnLoaded.Bind(callback);

		std::unique_ptr<CCFunction::AFunction> function = CCFunction::BindFunction(&CCCallback::AWrapCallback::Invoke,
			wrappedCallback, resource);

		m_threadpool->CreateTask(function, EChannelTask::MAINTHREAD);
	}
	break;

	default:
		break;
	}
}

template<class T, typename... Args>
std::shared_ptr<T> ResourceManager::AddResourceMultiThreads(const char* filepath, bool verifIsExist,
	std::shared_ptr<CCCallback::ACallback<std::shared_ptr<T>>> callback, Args&&... args)
{
	std::shared_ptr<T>& resource = AddResourceRef<T>(filepath, verifIsExist);

	std::shared_ptr<CCCallback::AWrapCallback> wrappedCallback(nullptr);

	if (callback)
	{
		//Wrap callback because CCFunction::BindFunction(...) doesn't work with an arg<Type<OtherType>>
		wrappedCallback = std::dynamic_pointer_cast<CCCallback::AWrapCallback>(callback);
	}

	auto function =
		CCFunction::BindFunctionUnsafe(
			&ResourceManager::AddResourceWithCallback<T, Args...>, this,
			resource, wrappedCallback, args...);

	m_threadpool->CreateTask(function, EChannelTask::MULTITHREAD);

	return resource;
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

template<class T, typename... Args>
void ResourceManager::Reload(const char* filepath, Args... args)
{
	auto resourceContainerIt = m_resources.find(typeid(T));
	if (resourceContainerIt != m_resources.end())
	{
		resourceContainerIt->second->Reload<T>(filepath, args...);
	}
}

template<class T>
void ResourceManager::Rename(const std::filesystem::path& filepath, const char* newFilepath)
{
	auto resourceContainerIt = m_resources.find(typeid(T));
	if (resourceContainerIt != m_resources.end())
	{
		resourceContainerIt->second->Rename(filepath, newFilepath);
	}
}


