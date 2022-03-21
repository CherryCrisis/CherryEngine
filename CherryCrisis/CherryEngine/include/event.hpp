#pragma once

#include <set>
#include <memory>
#include <typeindex>

#include "callback.hpp"

template<class... Args>
class Event
{
private:
	using ACallback = CCCallback::ACallback<Args...>;

	std::set<ACallback*> m_callbacks;
public:
	Event() = default;
	virtual ~Event()
	{
		for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
			delete *it;
	}

	void Bind(std::unique_ptr<CCCallback::ACallback<Args...>>& callback)
	{
		m_callbacks.insert(callback.release());
	}

	template<typename T>
	void Bind(void (T::* func)(Args... type), T* member)
	{
		std::unique_ptr<ACallback> callback = CCCallback::BindCallback(func, member);
		Bind(callback);
	}

	void Bind(void (*func)(Args... type))
	{
		std::unique_ptr<ACallback> callback = CCCallback::BindCallback(func);
		Bind(callback);
	}

	template<typename T>
	void Unbind(void (T::* func)(Args... type), T* member)
	{
		for (ACallback* callback : m_callbacks)
		{
			if (auto memberCallback = static_cast<CCCallback::MemberCallback<T, Args...>*>(callback))
			{
				if (memberCallback->m_func == func && memberCallback->m_member == member)
				{
					m_callbacks.erase(callback);
					delete callback;

					return;
				}
			}
		}
	}

	void Unbind(void (*func)(Args... type))
	{
		for (ACallback* callback : m_callbacks)
		{
			if (auto nonMemberCallback = static_cast<CCCallback::NonMemberCallback<Args...>*>(callback))
			{
				if (nonMemberCallback->m_func == func)
				{
					m_callbacks.erase(callback);
					delete callback;
					return;
				}
			}
		}
	}

	void Invoke(Args&&... args)
	{
		for (ACallback* callback : m_callbacks)
		{
			callback->Invoke(std::forward<Args>(args)...);
		}
	}
};
