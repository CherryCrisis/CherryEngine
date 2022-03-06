#pragma once

#include <tuple>

namespace CCCallback
{
	template<class... Args>
	class ACallback
	{
	public:
		virtual void Invoke(Args&&...) const = 0;
	};

	template<class T, class... Args>
	class MemberCallback : public ACallback<Args...>
	{
	private:
		void (T::* m_func)(Args... type);
		T* m_member;

	public:
		MemberCallback(void (T::* func)(Args... type), T* c)
			: m_func(func), m_member(c)
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
	private:
		void (*m_func)(Args... type);

	public:
		NonMemberCallback(void (*func)(Args... type))
			: m_func(func)
		{
		}

		virtual void Invoke(Args&&... args) const override
		{
			(*m_func)(args...);
		}
	};
}
