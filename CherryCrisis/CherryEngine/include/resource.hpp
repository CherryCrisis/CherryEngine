#pragma once

#include <string>
#include <memory>
#include <atomic>

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
	const std::string m_filepath;
	std::atomic<EResourceState> m_resourceState;

public:
	AResource(const std::string& filepath)
		: m_filepath(filepath)
	{
		m_resourceState.store(EResourceState::EMPTY);
	}

	virtual ~AResource() = default;

	const char* GetFilepath() const { return m_filepath.c_str(); }

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
		m_onLoaded.Invoke(std::move(resourcePtrCopy));
	}

	

public:
	Event<std::shared_ptr<T>> m_onLoaded{};
	Event<> m_onDestroyed {};

	Resource(const std::string& filepath)
		: AResource(filepath) {}

	virtual ~Resource()
	{
		IsDestroyed(ThreadPool::GetInstance());
	}

	void IsLoaded(std::shared_ptr<T> resource, ThreadPool* m_threadpool)
	{
		auto eventFunction = CCFunction::BindFunctionUnsafe(&Resource::OnLoaded<T>, this, resource);
		m_threadpool->CreateTask(eventFunction, EChannelTask::MAINTHREAD);
	}

	void IsDestroyed(ThreadPool* m_threadpool)
	{
		auto eventFunction = CCFunction::BindFunctionUnsafe(&Event<>::Invoke, &this->m_onDestroyed);
		m_threadpool->CreateTask(eventFunction, EChannelTask::MAINTHREAD);
	}
};

