#include <pch.hpp>

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
	Debug* debug = currentInstance;

	LogMessage* logMessage = nullptr;

	size_t key;
	debug->GetKeyOfLog(key, message, logType);

	auto conditional = debug->m_logMessages.find(key);
	if (conditional != debug->m_logMessages.end())
	{
		conditional->second.m_count++;
		logMessage = &conditional->second;
	}
	else
	{
		auto pair = debug->m_logMessages.emplace(key, LogMessage(message, logType));
		logMessage = &pair.first->second;
	}

	debug->m_logTypeCounts[(int)logType]++;

	debug->m_logs.emplace_back(logMessage, debug->m_timeManager->GetCurrentTime(), line, file, function);

	debug->m_isLogAdded = true;
}

void Debug::Clear()
{
	m_logs.clear();
	m_logMessages.clear();

	for (int i = 0; i < 3; ++i)
		m_logTypeCounts[i] = 0;
}
