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
		ACallback callback = CCCallback::BindCallback(func, member);
		m_callbacks.insert(std::make_pair<const std::type_index&, ACallback>(typeid(func), std::move(callback)));
	}

	void Bind(void (*func)(Args... type))
	{
		ACallback callback = CCCallback::BindCallback(func);
		m_callbacks.insert(std::make_pair<const std::type_index&, ACallback>(typeid(func), std::move(callback)));
	}

	void Bind(ACallback& callback, const std::type_index& typeID)
	{
		m_callbacks.insert(std::make_pair<const std::type_index&, ACallback>(typeID, std::move(callback)));
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
		if (m_callbacks.empty())
		{
			for (std::pair<const std::type_index, ACallback>& callback : m_callbacks)
			{
				callback.second->Invoke(std::forward<Args>(args)...);
			}
		}
	}
};
