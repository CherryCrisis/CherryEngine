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
		for (auto callbackIt = m_callbacks.begin(); callbackIt != m_callbacks.end(); callbackIt++)//ACallback* callback : m_callbacks)
		{
			ACallback* callback = *callbackIt;

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
		for (auto callbackIt = m_callbacks.begin(); callbackIt != m_callbacks.end(); callbackIt++)//ACallback* callback : m_callbacks)
		{
			ACallback* callback = *callbackIt;

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
		//TODO: Optimize, iterator crash if there is an unbind

		std::set<ACallback*> callbacks = m_callbacks;
		for (auto callbackIt = callbacks.begin(); callbackIt != callbacks.end(); callbackIt++)
		{
			ACallback* callback = *callbackIt;
			callback->Invoke(std::forward<Args>(args)...);
		}
	}
};
