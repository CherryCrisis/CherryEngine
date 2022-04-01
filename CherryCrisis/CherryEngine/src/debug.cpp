#include "pch.hpp"
#include "debug.hpp"

template <>
Debug* Singleton<Debug>::currentInstance = nullptr;

Debug::Debug() 
{
	m_timeManager = TimeManager::GetInstance();
	AddLog("Test", ELogType::ERROR);
	AddLog("Test2", ELogType::ERROR);
	AddLog("Test2", ELogType::WARNING);
	AddLog("Test", ELogType::INFO);
	AddLog("Test2", ELogType::WARNING);
	AddLog("Test4", ELogType::INFO);
	AddLog("Test2", ELogType::WARNING);
}

void Debug::AddLog(const char* message, ELogType logType)
{
	std::cout << message << std::endl;

	LogMessage* logMessage = nullptr;

	size_t key;
	GetKeyOfLog(key, message, logType);

	auto conditional = m_logMessages.find(key);
	if (conditional != m_logMessages.end())
	{
		conditional->second.m_count++;
		logMessage = &conditional->second;
	}
	else
	{
		auto pair = m_logMessages.emplace(key, LogMessage(message, logType ));
		logMessage = &pair.first->second;
	}
	
	m_logs.emplace_back(logMessage, m_timeManager->GetCurrentTime());
}

void Debug::Clear()
{
	m_logs.clear();
}
