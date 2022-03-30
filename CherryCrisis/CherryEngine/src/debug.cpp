#include "pch.hpp"
#include "debug.hpp"

template <>
Debug* Singleton<Debug>::currentInstance = nullptr;

Debug::Debug() 
{
	m_timeManager = TimeManager::GetInstance();



	/*HashTest hashTest{ .logType = ELogType::ERROR, .str = "test" };

	std::size_t key = std::hash<HashTest>{}(hashTest);*/
}


void Debug::AddLog(const char* message, ELogType logType)
{
	std::cout << message << std::endl;

	LogMessage* logMessage = nullptr;

	auto conditional = m_logMessages.find(message);
	if (conditional != m_logMessages.end())
	{
		conditional->second.count++;
		logMessage = &conditional->second;
	}
	else
	{
		auto pair = m_logMessages.emplace(message, LogMessage());
		logMessage->m_logMessage = &pair.first->first; //To avoid copy string

		logMessage = &pair.first->second;
	}
	
	m_logs.push_back(Log(logMessage, logType, m_timeManager->GetCurrentTime()));
}

void Debug::Clear()
{
	m_logs.clear();
}
