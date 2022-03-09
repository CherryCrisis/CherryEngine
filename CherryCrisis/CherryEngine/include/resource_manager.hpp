#pragma once

#include <memory>
#include <typeinfo>
#include <typeindex>
#include <functional>

#include "engine.hpp"

#include "resource.hpp"
#include "callback.hpp"

class CCENGINE_API ResourceManager
{
private:
	std::unordered_multimap<std::type_index, std::shared_ptr<Resource>> m_resources;
	static ResourceManager* m_instance;

public:
	static ResourceManager* GetInstance();

	template<class T = Resource, class CallbackType, typename... Args>
	std::shared_ptr<T> AddResourceMultiThreads(const char* filepath, bool verifIsExist,
		std::unique_ptr<CCCallback::ACallback<CallbackType>> callback, Args... args);

	template<class T = Resource, typename... Args>
	std::shared_ptr<T> AddResourceMonoThreads(const char* filepath, bool verifIsExist, Args... args);

	template<class T = Resource, typename... Args>
	std::shared_ptr<T> AddResource(Args... args);

	template<class T = Resource>
	std::shared_ptr<T> GetResource(const char* filepath) const;

	template<class T = Resource>
	void GetAllResources(std::vector<std::shared_ptr<T>>& resources) const;

	size_t GetResourceCount() const { return m_resources.size(); }
};

#include "resource_manager.inl"
