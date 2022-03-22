#pragma once

#include <string>
#include <memory>
#include <atomic>

#include "cherry_macros.hpp"
#include "event.hpp"
#include "threadpool.hpp"

class CCENGINE_API AResource
{
protected:
	const size_t hashId;
	std::atomic<bool> m_isLoaded;

public:
	const std::string filepath;
	AResource(const std::string& filepath)
		: hashId(std::hash<std::string>()(filepath)), filepath(filepath) {}

	virtual ~AResource() = default;

	const size_t GetHashId() const { return hashId; }
	const char* GetFilepath() const { return filepath.c_str(); }

	bool GetIsLoaded() { return m_isLoaded.load(); }
};

template<class T>
class Resource : public AResource
{
protected:
	template<class ResourceT>
	void OnLoaded(std::shared_ptr<ResourceT> resource)
	{
		m_isLoaded.store(true);

		std::shared_ptr<ResourceT> resourcePtrCopy = resource;
		m_onLoaded.Invoke(std::move(resourcePtrCopy));
	}

public:
	Event<std::shared_ptr<T>> m_onLoaded {};

	Resource(const std::string& filepath)
		: AResource(filepath) {}

	virtual ~Resource() = default;

	void IsLoaded(std::shared_ptr<T> resource, ThreadPool* threadpool)
	{
		auto eventFunction = CCFunction::BindFunctionUnsafe(&Resource::OnLoaded<T>, this, resource);
		threadpool->CreateTask(eventFunction, EChannelTask::MAINTHREAD);
	}
};

