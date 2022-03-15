#pragma once

#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>

#include <vector>
#include <memory>

#include "function.hpp"
#include "callback.hpp"
#include "event.hpp"

enum class EChannelTask
{
	Multithread,
	MainThread
};

class LockGuard
{
	std::shared_ptr<std::mutex> m_mutex;

public :
	LockGuard(std::shared_ptr<std::mutex>& mutex)
		: m_mutex(mutex) 
	{
		m_mutex->lock();
	}

	~LockGuard()
	{
		m_mutex->unlock();
	}
};

class Task
{
public:
	using AFunction = std::unique_ptr<CCFunction::AFunction>;

	AFunction m_func;

	Task() = default;
	Task(AFunction& func) : m_func(std::move(func)) {}
};

class ThreadPool
{
private:
	std::queue<std::unique_ptr<Task>>	m_multiThreadsTasks;
	std::vector<std::thread>			m_threads;
	std::condition_variable				m_condition;
	std::mutex							m_multiThreadsQueueLock;
	bool								m_stopThreads = false;

	std::mutex							m_mainThreadQueueLock;
	std::queue<std::unique_ptr<Task>>	m_mainThreadTasks;

	static ThreadPool* m_instance;

public:

	ThreadPool();
	~ThreadPool();

	static ThreadPool* GetInstance();
	
	using AFunction = std::unique_ptr<CCFunction::AFunction>;

	void CreateTask(AFunction& function, EChannelTask channelTask);
	void Update(EChannelTask channelTask);
};
