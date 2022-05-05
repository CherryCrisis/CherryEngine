#pragma once

#include <memory>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <map>

#include "engine.hpp"
#include "singleton.hpp"
#include "threadpool.hpp"

#include "resource.hpp"
#include "callback.hpp"
#include "function.hpp" 
#include "resources_container.hpp"

class CCENGINE_API ResourceManager : public Singleton<ResourceManager>
{
private:
	std::map<std::type_index, std::unique_ptr<AResourcesContainer>> m_resources;

	std::mutex m_lockResources;

	ThreadPool* m_threadpool = nullptr;

	template<class T, typename... Args>
	void AddResourceWithCallback(std::shared_ptr<T>& resource,
		std::shared_ptr<CCCallback::AWrapCallback> wrappedCallback, Args... args);

	template<class T>
	std::shared_ptr<T>& CreateResource(const char* filepath);

public:

	ResourceManager();
	~ResourceManager();

	template<class T>
	std::shared_ptr<T>& AddResourceRef(const char* filepath, bool verifIsExist = true);

	template<class T, typename... Args>
	std::shared_ptr<T> AddResource(const char* filepath, bool verifIsExist, Args... args);

	template<class T, typename... Args>
	std::shared_ptr<T> AddResourceMultiThreads(const char* filepath, bool verifIsExist,
		std::shared_ptr<CCCallback::ACallback<std::shared_ptr<T>>> callback, Args&&... args);

	template<class T>
	std::shared_ptr<T> GetResource(const char* filepath);

	size_t GetResourceCount() const;
	
	template<class T>
	size_t GetResourceCount() const;

	//To draw filepath in editor (Resource Viewer)
	void GetResourcesPath(std::map<std::type_index, std::vector<std::filesystem::path*>> &resourcePaths) const;

	//Unload unused resources
	void Purge();

	template<class T>
	void Remove(const char* filepath);

	template<class T, typename... Args>
	void Reload(const char* filepath, Args... args);

	template<class T>
	void Rename(const std::filesystem::path& filepath, const char* newFilepath);
	void Rename(const std::type_index& typeID, const std::filesystem::path& filepath, const char* newFilepath);
};


#include "resource_manager.inl"
