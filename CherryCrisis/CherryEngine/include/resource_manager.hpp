#pragma once

#include <memory>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <map>

#include "engine.hpp"
#include "threadpool.hpp"

#include "resource.hpp"
#include "callback.hpp"
#include "function.hpp" 
#include "resources_container.hpp"

class CCENGINE_API ResourceManager
{
private:
	std::map<std::type_index, std::unique_ptr<AResourcesContainer>> m_resources;

	std::mutex m_lockResources;

	static ResourceManager* m_instance;

	ThreadPool* m_threadpool = nullptr;

	template<class T, class CallbackType, typename... Args>
	void AddResourceWithCallback(const char* filepath, bool verifIsExist,
		CCCallback::AWrapCallback* wrappedCallback, Args... args);

	template<class T>
	std::shared_ptr<T> CreateResource(const char* filepath);

public:
	static ResourceManager* GetInstance();

	ResourceManager();
	~ResourceManager();

	template<class T>
	std::shared_ptr<T> AddResourceRef(const char* filepath, bool verifIsExist = true);

	template<class T, typename... Args>
	std::shared_ptr<T> AddResource(const char* filepath, bool verifIsExist, Args... args);

	template<class T, class CallbackType, typename... Args>
	void AddResourceMultiThreads(const char* filepath, bool verifIsExist,
		std::unique_ptr<CCCallback::ACallback<CallbackType>>& callback, Args&&... args);

	template<class T>
	std::shared_ptr<T> GetResource(const char* filepath);

	size_t GetResourceCount() const { return m_resources.size(); }

	//Unload unused resources
	void Purge();

	template<class T>
	void Remove(const char* filepath);
};


#include "resource_manager.inl"
