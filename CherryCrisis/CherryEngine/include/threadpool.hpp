#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include <cherry_macros.hpp>

#include "singleton.hpp"

#include "callback.hpp"

#include "event.hpp"
#include "function.hpp"


enum class EChannelTask
{
	MULTITHREAD,
	MAINTHREAD
};

class Task
{
public:
	std::unique_ptr<CCFunction::AFunction> m_func;

	Task() = default;
	Task(std::unique_ptr<CCFunction::AFunction>& func) : m_func(std::move(func)) {}

	virtual ~Task() = default;
};

class CCENGINE_API ThreadPool : public Singleton<ThreadPool>
{
private:
	friend class Singleton<ThreadPool>;

	bool								m_stopThreads = false;

	std::vector<std::thread>			m_threads;
	std::condition_variable				m_condition;
	std::mutex							m_multiThreadsQueueLock;
	std::queue<std::unique_ptr<Task>>	m_multiThreadsTasks;

	std::mutex							m_mainThreadQueueLock;
	std::queue<std::unique_ptr<Task>>	m_mainThreadTasks;

	std::thread::id m_mainThreadID;

public:
	ThreadPool();
	virtual ~ThreadPool();

	void RethrowExceptions(std::exception_ptr exeption);
	void CreateTask(std::unique_ptr<CCFunction::AFunction>& function, EChannelTask channelTask);
	void Update(EChannelTask channelTask);

	std::thread::id GetMainThreadID() { return m_mainThreadID; }
};
