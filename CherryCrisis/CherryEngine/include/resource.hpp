#pragma once

#include <string>
#include <memory>
#include <atomic>
#include <filesystem>

#include "cherry_macros.hpp"
#include "event.hpp"
#include "threadpool.hpp"


enum class EResourceState
{
	EMPTY,
	LOADING,
	LOADED,
	DESTROYED,
};

class CCENGINE_API AResource
{
protected:
	std::atomic<EResourceState> m_resourceState;

	virtual void Delete() {};

public:
	std::filesystem::path m_filepath;

	AResource(const std::filesystem::path& filepath)
		: m_filepath(filepath)
	{
		m_resourceState.store(EResourceState::EMPTY);
	}

	virtual ~AResource() = default;

	std::string GetFilepath() { return m_filepath.string(); }
	std::filesystem::path* GetFilesystemPath() { return &m_filepath; }

	EResourceState GetResourceState() { return m_resourceState.load(); }
	void SetResourceState(EResourceState resourceState) { m_resourceState.store(resourceState); }

};

template<class T>
class Resource : public AResource
{
protected:
	template<class ResourceT>
	void OnLoaded(std::shared_ptr<ResourceT> resource)
	{
		m_resourceState.store(EResourceState::LOADED);

		std::shared_ptr<ResourceT> resourcePtrCopy = resource;
		m_OnLoaded.Invoke(std::move(resourcePtrCopy));
	}

public:
	Event<std::shared_ptr<T>> m_OnLoaded {};
	Event<> m_OnReloaded {};
	Event<> m_OnDeleted {};

	Resource(const std::filesystem::path& filepath)
		: AResource(filepath) 
	{
	}

	virtual ~Resource() 
	{
		if (m_resourceState.load() != EResourceState::DESTROYED)
			Delete();
	}

	void IsLoaded(std::shared_ptr<T> resource, ThreadPool* m_threadpool)
	{
		auto eventFunction = CCFunction::BindFunctionUnsafe(&Resource::OnLoaded<T>, this, resource);
		m_threadpool->CreateTask(eventFunction, EChannelTask::MAINTHREAD);
	}

	void DeleteResource()
	{
		m_resourceState.store(EResourceState::DESTROYED);

		Delete();
		m_OnDeleted.Invoke();
	}

	template<typename... Args>
	static void ReloadResource(std::shared_ptr<T> resource, Args... args)
	{
		resource->SetResourceState(EResourceState::LOADING);
		resource->Reload(args...);
		resource->m_OnReloaded.Invoke();
		resource->SetResourceState(EResourceState::LOADED);
	}
};

