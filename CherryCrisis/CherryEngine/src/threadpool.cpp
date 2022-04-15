#include "pch.hpp"

#include "threadpool.hpp"  

#include "debug.hpp"

ThreadPool* ThreadPool::m_instance = nullptr;

ThreadPool* ThreadPool::GetInstance()
{
	if (!m_instance)
		m_instance = new ThreadPool();

	return m_instance;
}

ThreadPool::ThreadPool()
{
	m_mainThreadID = std::this_thread::get_id();

	const int threadCount = (std::thread::hardware_concurrency() / 2) - 1;
	std::cout << std::to_string(threadCount) << std::endl;

	for (int i = 0; i < threadCount; ++i)
	{
		std::thread slave(&ThreadPool::Update, this, EChannelTask::MULTITHREAD);
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

void ThreadPool::CreateTask(std::unique_ptr<CCFunction::AFunction>& task, EChannelTask channelTask)
{
	if (channelTask == EChannelTask::MULTITHREAD)
	{
		std::unique_lock<std::mutex> queueLock(m_multiThreadsQueueLock);
		m_multiThreadsTasks.push(std::make_unique<Task>(task));
		m_condition.notify_one();
	}
	else
	{
		std::unique_lock<std::mutex> queueLock(m_mainThreadQueueLock);
		m_mainThreadTasks.push(std::make_unique<Task>(task));
	}
}

void ThreadPool::Update(EChannelTask channelTask)
{
	if (channelTask == EChannelTask::MULTITHREAD)
	{
		while (!m_stopThreads)
		{
			std::unique_ptr<Task> task;
			{
				std::unique_lock<std::mutex> queueLock(m_multiThreadsQueueLock);

				while (!m_stopThreads && m_multiThreadsTasks.empty())
				{
					m_condition.wait(queueLock);
				}

				if (m_stopThreads)
					break;

				task = std::move(m_multiThreadsTasks.front());
				m_multiThreadsTasks.pop();
			}



			try
			{
				task->m_func->Invoke();
			}
			catch (const std::exception& exception)
			{
				std::exception_ptr exceptionPtr = std::current_exception();
				auto exeptionFunc = CCFunction::BindFunctionUnsafe(&ThreadPool::RethrowExceptions, this, exceptionPtr);
				CreateTask(exeptionFunc, EChannelTask::MAINTHREAD);
			}
		}
	}
	else
	{
		std::unique_ptr<Task> task;

		while (!m_mainThreadTasks.empty())
		{
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
}

void ThreadPool::RethrowExceptions(std::exception_ptr exceptionPtr)
{
	try
	{
		std::rethrow_exception(exceptionPtr);
	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
	}
}


