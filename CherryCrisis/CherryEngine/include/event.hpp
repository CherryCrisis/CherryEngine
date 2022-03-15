#pragma once

#include <map>
#include <memory>
#include <typeindex>

#include "callback.hpp"

template<class... Args>
class Event
{
private:
	using ACallback = CCCallback::ACallback<Args...>;

	std::map<const std::type_index, ACallback*> m_callbacks;

public :
	virtual ~Event() = default;

public:
	Event() = default;

	void Bind(std::unique_ptr<ACallback>& callback, const std::type_index& typeID)
	{
		m_callbacks.insert(std::make_pair<const std::type_index&, ACallback*>(typeID, std::move(callback.release())));
	}

	template<typename T>
	void Bind(void (T::* func)(Args... type), T* member)
	{
		std::unique_ptr<ACallback> callback = CCCallback::BindCallback(func, member);
		Bind(callback, typeid(func));
	}

	void Bind(void (*func)(Args... type))
	{
		std::unique_ptr <ACallback> callback = CCCallback::BindCallback(func);
		Bind(callback, typeid(func));
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
		{
			delete callback.second;
			m_callbacks.erase(funcId);
		}
		//else
		//TODO: add debug log
	}

	template<class... Args>
	void Invoke(Args&&... args)
	{
		for (std::pair<const std::type_index, ACallback*>& callback : m_callbacks)
		{
			callback.second->Invoke(std::forward<Args>(args)...);
		}
	}
};
