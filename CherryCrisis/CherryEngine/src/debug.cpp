#include "pch.hpp"
#include "debug.hpp"

template <>
Debug* Singleton<Debug>::currentInstance = nullptr;

Debug::Debug() 
{
	m_timeManager = TimeManager::GetInstance();

	AddLog(ELogType::INFO, "rfjpigdpjiodfo");
	AddLog(ELogType::INFO, "rfjpigdpjiodfo");
	AddLog(ELogType::INFO, "rfjpigdpjiodfo");
	AddLog(ELogType::INFO, "rfjpigdpjiodfo");
}

void Debug::AddLog(ELogType logType, const char* message, std::source_location location)
{
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

	m_logTypeCounts[(int)logType]++;

	m_logs.emplace_back(logMessage, m_timeManager->GetCurrentTime(), location);
}

void Debug::Clear()
{
	m_logs.clear();
	m_logMessages.clear();

	for (int i = 0; i < 3; ++i)
		m_logTypeCounts[i] = 0;
}
