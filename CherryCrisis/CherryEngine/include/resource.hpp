#pragma once

#include <string>
#include <memory>
#include <atomic>

#include "cherry_macros.hpp"
#include "event.hpp"
#include "threadpool.hpp"

class CCENGINE_API WrapResource
{
public :
	WrapResource() = default;
	virtual ~WrapResource() = default;
};

template<class T>
class CCENGINE_API Resource : public WrapResource
{
protected:
	const size_t hashId;
	const std::string filepath;
	std::atomic<bool> m_isLoaded;
	
	void OnLoaded(std::shared_ptr<T> resource)
	{
		m_isLoaded.store(true);
		m_onLoaded.Invoke(resource);
	}

public:
	Event<std::shared_ptr<T>> m_onLoaded {};

	Resource(const std::string& filepath)
		: hashId(std::hash<std::string>()(filepath)), filepath(filepath) {}

	virtual ~Resource() = default;

	const size_t GetHashId() const { return hashId; }
	const char* GetFilepath() const { return filepath.c_str(); }

	void IsLoaded(std::shared_ptr<T> resource, ThreadPool* threadpool)
	{
		auto eventFunction = CCFunction::BindFunctionUnsafe(&Resource::OnLoaded, resource.get(), resource);
		threadpool->CreateTask(eventFunction, EChannelTask::MAINTHREAD);
	}

	bool GetIsLoaded() { return m_isLoaded.load(); }

};

