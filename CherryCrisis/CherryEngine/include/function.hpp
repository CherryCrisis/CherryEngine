#pragma once

#include <tuple>
#include <memory>

namespace CCFunction
{
	class AFunction
	{
	public:
		virtual void Invoke() const = 0;
	};

	template<class... Args>
	class NonMemberFunction : public AFunction
	{
	private:
		void (*m_f)(Args... type);
		std::tuple<Args&&...> m_args;

	public:
		NonMemberFunction(void (*f)(Args... type), std::tuple<Args...> args)
			: m_f(f), m_args(args)
		{
		}

		virtual void Invoke() const override
		{
			(m_f)(std::get<Args>(m_args)...);
		}
	};

	template<class T, class... Args >
	class MemberFunction : public AFunction
	{
	private:
		void (T::* m_f)(Args... type);
		T* m_member;
		std::tuple<Args&&...> m_args;

	public:
		MemberFunction(void (T::* f)(Args... type), T* c, std::tuple<Args...> args)
			: m_f(f), m_member(c), m_args(args)
		{
		}

		virtual void Invoke() const override
		{
			(m_member->*m_f)(std::get<Args>(m_args)...);
		}
	};

	template<class T, class... Args>
	std::unique_ptr<AFunction> BindFunction(void (T::* f)(Args... type), T* c, Args&&... args)
	{
		std::unique_ptr<MemberFunction<T, Args...>> function =
			std::make_unique<MemberFunction<T, Args...>>(MemberFunction(f, c, std::forward_as_tuple<Args>(args)...));

		return std::move(function);
	}

	template<class... Args>
	constexpr std::unique_ptr<AFunction> BindFunction(void (*f)(Args... type), Args&&... args)
	{
		std::unique_ptr<NonMemberFunction<Args...>> function =
			std::make_unique<NonMemberFunction<Args...>>(NonMemberFunction(f, std::forward_as_tuple<Args>(args)...));

		return std::move(function);
	}
}
