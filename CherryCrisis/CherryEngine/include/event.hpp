#pragma once

#include <set>
#include <memory>
#include <typeindex>
#include <queue>

#include "callback.hpp"
#include "function.hpp"

template<class... Args>
class Event
{
private:
	std::set<std::shared_ptr<CCCallback::ACallback<Args...>>> m_callbacks;
	std::queue<CCFunction::AFunction*> m_queries;

	void AddToCallbacks(std::shared_ptr<CCCallback::ACallback<Args...>> callback)
	{
		m_callbacks.insert(callback);
	}

	void DeleteCallbacks(std::shared_ptr<CCCallback::ACallback<Args...>> callback)
	{
		m_callbacks.erase(callback);
	}

public:
	Event() = default;
	virtual ~Event() = default;


	void Bind(std::shared_ptr<CCCallback::ACallback<Args...>> callback)
	{
		auto function = CCFunction::BindFunctionUnsafe(&Event<Args...>::AddToCallbacks, this, callback);
		m_queries.push(function.release());
	}

	template<typename T>
	void Bind(void (T::* func)(Args... type), T* member)
	{
		auto callback = CCCallback::BindCallback(func, member);
		Bind(callback);
	}

	void Bind(void (*func)(Args... type))
	{
		auto callback = CCCallback::BindCallback(func);
		Bind(callback);
	}

	template<typename T>
	void Unbind(void (T::* func)(Args... type), T* member)
	{
		std::shared_ptr<CCCallback::ACallback<Args...>> findedCallback(nullptr);
		for (auto& callback : m_callbacks)
		{
			if (auto memberCallback = std::dynamic_pointer_cast<CCCallback::MemberCallback<T, Args...>>(callback))
			{
				if (memberCallback->m_func == func && memberCallback->m_member == member)
				{
					findedCallback = callback;
					break;
				}
			}
		}

		if (findedCallback)
		{
			auto function = CCFunction::BindFunctionUnsafe(&Event<Args...>::DeleteCallbacks, this, findedCallback);
			m_queries.push(function.release());
		}
	}

	void Unbind(void (*func)(Args... type))
	{
		std::shared_ptr<CCCallback::ACallback<Args...>> findedCallback(nullptr);
		for (auto& callback : m_callbacks)
		{
			if (auto nonMemberCallback = std::dynamic_pointer_cast<CCCallback::NonMemberCallback<Args...>>(callback))
			{
				if (nonMemberCallback->m_func == func)
				{
					findedCallback = callback;
					return;
				}
			}
		}

		if (findedCallback)
		{
			auto function = CCFunction::BindFunctionUnsafe(&Event<Args...>::DeleteCallbacks, this, findedCallback);
			m_queries.push(function.release());
		}
	}

	void Invoke(Args&&... args)
	{
		while (!m_queries.empty())
		{
			auto& function = m_queries.front();
			function->Invoke();
			m_queries.pop();
		}

		for (auto callbackIt = m_callbacks.begin(); callbackIt != m_callbacks.end(); callbackIt++)
		{
			std::shared_ptr< CCCallback::ACallback<Args...>> callback = *callbackIt;
			callback->Invoke(std::forward<Args>(args)...);
		}
	}

	void Reset()
	{
		m_callbacks.clear();
	}
};
