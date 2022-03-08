#include "pch.hpp"
#include "threadpool.hpp"

ThreadPool* ThreadPool::m_instance = nullptr;

ThreadPool* ThreadPool::GetInstance()
{
	if (!m_instance)
		m_instance = new ThreadPool();

	return m_instance;
}

ThreadPool::ThreadPool()
{
	const int threadCount = std::thread::hardware_concurrency() - 1;
	for (int i = 0; i < threadCount; ++i)
	{
		std::thread slave(&ThreadPool::Update, this, EChannelTask::Multithread);
		m_threads.push_back(std::move(slave));
	}
}

ThreadPool::~ThreadPool()
{
	m_stopThreads = true;
	while (m_threads.size() != 0)
	{
		m_threads[m_threads.size() - 1].join();
		m_threads.pop_back();
	}
}

void ThreadPool::CreateTask(std::unique_ptr<CCFunction::AFunction> function, EChannelTask channelTask)
{
	if (channelTask == EChannelTask::Multithread)
	{
		std::unique_lock<std::mutex> queueLock(m_multiThreadsQueueLock);
		m_multiThreadsTasks.push(std::make_unique<Task>(Task(function)));
		m_condition.notify_one();
	}
	else
	{
		std::unique_lock<std::mutex> queueLock(m_mainThreadQueueLock);
		m_mainThreadTasks.push(std::make_unique<Task>(Task(function)));
	}
}

void ThreadPool::Update(EChannelTask channelTask)
{
	if (channelTask == EChannelTask::Multithread)
	{
		while (!m_stopThreads)
		{
			std::unique_ptr<Task> task;
			{
				std::unique_lock<std::mutex> queueLock(m_multiThreadsQueueLock);

				if (!m_stopThreads && m_multiThreadsTasks.empty())
				{
					m_condition.wait(queueLock);
				}

				if (m_stopThreads)
					break;

				task = std::move(m_multiThreadsTasks.front());
				m_multiThreadsTasks.pop();
			}

			task->m_func->Invoke();

			//Add invoke event in mainthead queue
			//std::unique_lock<std::mutex> queueLock(m_mainThreadQueueLock);
			//task->m_func = CCFunction::BindFunction(&Event<>::Invoke, &task->m_onFinished);
			//m_mainThreadTasks.push(task);
		}
	}
	else
	{
		std::unique_ptr<Task> task;

		{
			std::unique_lock<std::mutex> queueLock(m_mainThreadQueueLock);

			if (m_mainThreadTasks.empty())
				return;

			task = std::move(m_mainThreadTasks.front());
			m_mainThreadTasks.pop();
		}

		task->m_func->Invoke();
	}
}


