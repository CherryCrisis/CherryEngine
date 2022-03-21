#pragma once

#pragma warning( disable : 4661 )

#include <atomic>

template <class T>
class Singleton
{
private:
	static std::atomic_flag instantiateFlag;

protected:
	Singleton() = default;

	static T* currentInstance;

public:
	static T* GetInstance()
	{
		if (!instantiateFlag.test_and_set())
		{
			if (!currentInstance)
				currentInstance = new T();
		}

		return currentInstance;
	}

	static void Kill()
	{
		delete currentInstance;
		currentInstance = nullptr;
		instantiateFlag.clear();
	}

	Singleton(T&) = delete;
	void operator=(const T&) = delete;
};

template <class T>
std::atomic_flag Singleton<T>::instantiateFlag = ATOMIC_FLAG_INIT;
