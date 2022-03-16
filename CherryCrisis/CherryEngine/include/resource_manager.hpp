#pragma once

#include <memory>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include <condition_variable>
#include <mutex>

#include <iostream>

#include "engine.hpp"
#include "threadpool.hpp"

#include "resource.hpp"
#include "callback.hpp"
#include "function.hpp" 


class CCENGINE_API ResourceManager
{
private:
	std::unordered_multimap<std::type_index, std::shared_ptr<Resource>> m_resources;

	std::mutex m_lockResources;

	static ResourceManager* m_instance;

	ThreadPool* threadpool = nullptr;

	template<class T = Resource, class CallbackType, typename... Args>
	void AddResourceWithCallback(const char* filepath, bool verifIsExist,
		CCCallback::AWrapCallback* wrappedCallback, Args... args);

	template<class T>
	Resource::Ref<T> CreateResource(const char* filepath);

public:
	static ResourceManager* GetInstance();

	ResourceManager();
	~ResourceManager();

	template<class T = Resource, typename... Args>
	std::shared_ptr<T> AddResource(const char* filepath, bool verifIsExist, Args... args);

	template<class T = Resource, class CallbackType, typename... Args>
	void AddResourceMultiThreads(const char* filepath, bool verifIsExist,
		std::unique_ptr<CCCallback::ACallback<CallbackType>>& callback, Args&&... args);

	template<class T = Resource>
	std::shared_ptr<T> GetResource(const char* filepath);

	template<class T = Resource>
	void GetAllResources(std::vector<std::shared_ptr<T>>& resources) const;

	size_t GetResourceCount() const { return m_resources.size(); }

};


#include "resource_manager.inl"
