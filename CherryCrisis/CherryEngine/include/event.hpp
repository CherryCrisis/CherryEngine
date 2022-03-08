#pragma once

#include <map>
#include <memory>
#include <typeindex>

#include "callback.hpp"

template<class... Args>
class Event
{
private:
	using ACallback = std::unique_ptr<CCCallback::ACallback<Args...>>;

	std::map<const std::type_index, ACallback> m_callbacks;

public:
	Event() = default;

	template<typename T>
	void Bind(void (T::* func)(Args... type), T* member)
	{
		ACallback callback = std::make_unique<CCCallback::MemberCallback<T, Args&&...>>(func, member);
		m_callbacks.insert(std::make_pair<const std::type_index&, ACallback>(typeid(func), std::move(callback)));
	}

	void Bind(void (*func)(Args... type))
	{
		ACallback callback = std::make_unique<CCCallback::NonMemberCallback<Args&&...>>(func);
		m_callbacks.insert(std::make_pair<const std::type_index&, ACallback>(typeid(func), std::move(callback)));
	}

	template<typename T>
	void Unbind(void (T::* func)(Args... type))
	{
		const std::type_index id = typeid(func);
		Unbind(id);
	}

	void Unbind(void (*func)(Args... type))
	{
		const std::type_index id = typeid(func);
		Unbind(id);
	}

	void Unbind(const std::type_index& funcId)
	{
		auto callback = m_callbacks.find(funcId);
		
		if (callback != m_callbacks.end())
			m_callbacks.erase(funcId);
		//else
		//TODO: add debug log
	}

	template<class... Args>
	void Invoke(Args&&... args)
	{
		for (std::pair<const std::type_index, ACallback>& callback : m_callbacks)
		{
			callback.second->Invoke(args...);
		}
	}
};
