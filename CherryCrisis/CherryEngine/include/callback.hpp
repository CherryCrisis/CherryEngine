#pragma once

#include <tuple>
#include <memory>
#include <typeindex>

namespace CCCallback
{
	class AWrapCallback
	{
	public:
		virtual ~AWrapCallback() = default;
		
		template<class... Args>
		void Invoke(Args&&... args);
	};

	template<class... Args>
	class ACallback : public AWrapCallback
	{
	public:
		const std::type_index m_typeIndex;

		ACallback(const std::type_index& typeIndex) : m_typeIndex(typeIndex) {}
		virtual void Invoke(Args&&...) const = 0;

		virtual bool IsEqual(ACallback<Args...>* callback) const = 0;

	};

	template<class T, class... Args>
	class MemberCallback : public ACallback<Args...>
	{
	public:

		std::unique_ptr<T> m_uniqueMember;
		void (T::* m_func)(Args... type);
		T* m_member;

		MemberCallback(void (T::* func)(Args... type), T* c)
			: m_func(func), m_member(c), ACallback<Args...>(typeid(func))
		{
		}

		MemberCallback(void (T::* func)(Args... type), std::unique_ptr<T>& c)
			: m_func(func), m_uniqueMember(std::move(c)), m_member(m_uniqueMember.get()), ACallback<Args...>(typeid(func))
		{
		}

		virtual void Invoke(Args&&... args) const override
		{
			if (m_member)
				(m_member->*m_func)(args...);
		}

		bool IsEqual(ACallback<Args...>* callback) const override
		{
			if (CCCallback::MemberCallback<T, Args...>* memberCallback = dynamic_cast<CCCallback::MemberCallback<T, Args...>*>(callback))
			{
				if (memberCallback->m_func == m_func && memberCallback->m_member == m_member)
				{
					return true;
				}
			}

			return false;
		}

	};

	template<class... Args>
	class NonMemberCallback : public ACallback<Args...>
	{
	public:
		void (*m_func)(Args... type);

		NonMemberCallback(void (*func)(Args... type))
			: m_func(func), ACallback<Args...>(typeid(func))
		{
		}

		virtual void Invoke(Args&&... args) const override
		{
			(*m_func)(args...);
		}

		bool IsEqual(ACallback<Args...>* callback) const override
		{
			if (CCCallback::NonMemberCallback<Args...>* nonMemberCallback = dynamic_cast<CCCallback::NonMemberCallback<Args...>*>(callback))
			{
				if (nonMemberCallback->m_func == m_func)
				{
					return true;
				}
			}

			return false;
		}
	};

	template<class... Args>
	void AWrapCallback::Invoke(Args&&... args)
	{
		ACallback<Args...>* unwrapCallback = static_cast<ACallback<Args...>*>(this);
		unwrapCallback->Invoke(std::forward<Args>(args)...);
	}

	template<class T, class... Args>
	std::shared_ptr<ACallback<Args...>> BindCallback(void (T::* func)(Args... type), T* member)
	{
		return std::make_shared<CCCallback::MemberCallback<T, Args...>>(func, member);
	}

	template<class T, class... Args>
	std::shared_ptr<ACallback<Args...>> BindCallback(void (T::* func)(Args... type), std::unique_ptr<T>& member)
	{
		return std::make_shared<CCCallback::MemberCallback<T, Args...>>(func, member);
	}

	template<class... Args>
	std::shared_ptr<ACallback<Args...>> BindCallback(void (*func)(Args...))
	{
		return std::make_shared<CCCallback::NonMemberCallback<Args...>>(func);
	}
}
