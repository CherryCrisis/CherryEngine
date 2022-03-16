#pragma once

#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>

#include <vector>
#include <memory>

#include "cherry_macros.hpp"
#include "function.hpp"
#include "callback.hpp"
#include "event.hpp"

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

class CCENGINE_API ThreadPool
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
	virtual ~ThreadPool();

	static ThreadPool* GetInstance();

	void RethrowExceptions(std::exception_ptr exeption);
	void CreateTask(std::unique_ptr<CCFunction::AFunction>& function, EChannelTask channelTask);
	void Update(EChannelTask channelTask);
};
