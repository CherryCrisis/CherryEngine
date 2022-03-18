#pragma once

#include <tuple>
#include <memory>
#include <typeindex>

namespace CCCallback
{
	class AWrapCallback
	{
	public:
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
	};

	template<class T, class... Args>
	class MemberCallback : public ACallback<Args...>
	{
	public:
		void (T::* m_func)(Args... type);
		T* m_member;

		MemberCallback(void (T::* func)(Args... type), T* c)
			: m_func(func), m_member(c), ACallback<Args...>(typeid(func))
		{
		}

		virtual void Invoke(Args&&... args) const override
		{
			(m_member->*m_func)(args...);
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
	};

	template<class... Args>
	void AWrapCallback::Invoke(Args&&... args)
	{
		ACallback<Args...>* unwrapCallback = static_cast<ACallback<Args...>*>(this);
		unwrapCallback->Invoke(std::forward<Args>(args)...);
	}

	template<class T, class... Args>
	std::unique_ptr<ACallback<Args...>> BindCallback(void (T::* func)(Args... type), T* member)
	{
		return std::make_unique<CCCallback::MemberCallback<T, Args...>>(func, member);
	}

	template<class... Args>
	std::unique_ptr<ACallback<Args...>> BindCallback(void (*func)(Args...))
	{
		return std::make_unique<CCCallback::NonMemberCallback<Args...>>(func);
	}
}

#include <iostream>

//namespace CCTest
//{
//class TestStruct
//{
//public :
//	void FooTest(const int& i)
//	{
//		std::cout << "TEST" << std::endl;
//	}
//};
//}
