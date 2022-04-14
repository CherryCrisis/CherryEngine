#pragma once

#include <tuple>
#include <memory>

namespace CCFunction
{
	class AFunction
	{
	public:
		virtual void Invoke() = 0;
		virtual ~AFunction() = default;
	};

	template<class... Args>
	class NonMemberFunction : public AFunction
	{
	private:
		void (*m_f)(Args... type);
		std::tuple<Args...> m_args;

	public:
		constexpr NonMemberFunction(void (*f)(Args... type), Args&&... args)
			: m_f(f), m_args(std::forward<Args>(args)...)
		{
		}

		virtual void Invoke() override
		{
			(m_f)(std::get<Args>(m_args)...);
		}
	};

	template<class T, class... Args >
	class MemberFunction : public AFunction
	{
	private:
		//To support shared_ptr because when a function is 
		//bind with memberSharedPtr.get() the shared ptr may be destroy
		std::shared_ptr<T> m_memberRef;

		void (T::* m_f)(Args... m_type);
		T* m_member;
		std::tuple<Args...> m_args;

	public:
		MemberFunction(void (T::* f)(Args... type), T* c, Args&&... args)
			: m_f(f), m_member(c), m_args(std::forward<Args>(args)...)
		{
		}

		MemberFunction(void (T::* f)(Args... type), std::shared_ptr<T> c, Args&&... args)
			: m_f(f), m_member(c.get()), m_memberRef(c), m_args(std::forward<Args>(args)...)
		{
		}

		virtual void Invoke() override
		{
			(m_member->*m_f)(std::get<Args>(m_args)...);
		}
	};

	template<class T, typename... Args>
	std::unique_ptr<AFunction> BindFunction(void (T::* f)(Args... type), T* c, Args&&... args)
	{
		return std::make_unique<MemberFunction<T, Args...>>
			(f, c, std::forward<Args>(args)...);
	}

	template<class T, typename... Args>
	std::unique_ptr<AFunction> BindFunction(void (T::* f)(Args... type), std::shared_ptr<T> c, Args&&... args)
	{
		return std::make_unique<MemberFunction<T, Args...>>
			(f, c, std::forward<Args>(args)...);
	}

	template<class... Args>
	std::unique_ptr<AFunction> BindFunction(void (*f)(Args...), Args&&... args)
	{
		return std::make_unique<NonMemberFunction<Args...>>
			(f, std::forward<Args>(args)...);
	}

	//Unsafe because args of the function is not verified
	//(for exemple) use that for the variadic function
	template<typename... Args,  typename... TArgs>
	std::unique_ptr<AFunction> BindFunctionUnsafe(void (*f)(Args...), TArgs&&... args)
	{
		return std::make_unique<NonMemberFunction<Args...>>
			(f, std::forward<Args>(args)...);
	}

	//Unsafe because args of the function is not verified
	//(for exemple) use that for the variadic function
	template<class T, typename... Args, typename... TArgs>
	std::unique_ptr<AFunction> BindFunctionUnsafe(void (T::* f)(Args... type), T* c, TArgs&&... args)
	{
		return std::make_unique<MemberFunction<T, Args...>>
			(f, c, std::forward<Args>(args)...);
	}
}
