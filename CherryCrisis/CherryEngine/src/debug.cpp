#include "pch.hpp"
#include "debug.hpp"

template <>
Debug* Singleton<Debug>::currentInstance = nullptr;

Debug::Debug() 
{
	m_timeManager = TimeManager::GetInstance();
}

void Debug::AddLog(ELogType logType, const char* message, std::source_location location)
{
	AddLog(logType, message, location.line(), location.file_name(), location.function_name());
}

void Debug::AddLog(ELogType logType, const char* message, unsigned int line, const char* file, const char* function)
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
		auto pair = m_logMessages.emplace(key, LogMessage(message, logType));
		logMessage = &pair.first->second;
	}

	m_logTypeCounts[(int)logType]++;

	m_logs.emplace_back(logMessage, m_timeManager->GetCurrentTime(), line, file, function);
}

void Debug::Clear()
{
	m_logs.clear();
	m_logMessages.clear();

	for (int i = 0; i < 3; ++i)
		m_logTypeCounts[i] = 0;
}
