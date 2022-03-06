#pragma once

#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>

#include <vector>
#include <memory>

#include "function.hpp"
#include "event.hpp"

enum class EChannelTask
{
	Multithread,
	MainThread
};

class Task
{
public:
	std::unique_ptr<CCFunction::AFunction> m_func;
	Event<> m_onFinished = {};

	Task() = default;
	Task(std::unique_ptr<CCFunction::AFunction>& func) : m_func(std::move(func)) {}
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

	void CreateTask(std::unique_ptr<CCFunction::AFunction> function, EChannelTask channelTask);
	void Update(EChannelTask channelTask);
};
