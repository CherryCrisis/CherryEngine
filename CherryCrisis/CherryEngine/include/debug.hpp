#pragma once

#include "singleton.hpp"

#include <string>
#include <queue>
#include <unordered_map>

#include "cherry_macros.hpp"
#include "time_manager.hpp"

enum class CCENGINE_API ELogType
{
	INFO,
	WARNING,
	ERROR,
};

struct CCENGINE_API LogMessage
{
public:
	const std::string* m_logMessage;
	int count;

	//std::string CollapsedString() { std::string val = string; val += "    : " + std::to_string(count); return val; }
};

struct HashTest
{
	ELogType logType;
	std::string str;
};

class CCENGINE_API Log
{
private:
	LogMessage* m_logMessage;
	ELogType	m_logType;
	FullDate	m_date;

public:
	Log(LogMessage* logMessage, ELogType logType, FullDate fullDate)
		: m_logMessage(logMessage), m_logType(logType), m_date(fullDate) {}
};

class CCENGINE_API Debug : public Singleton<Debug>
{
private:
	std::unordered_map<std::string, LogMessage>		m_logMessages;
	std::vector<Log>								m_logs;

	TimeManager* m_timeManager;

public:
	Debug();

	std::vector<Log>* GetLogs() { return &m_logs; }
	
	// Print a string in the console and cache it
	void AddLog(const char* message, ELogType logType);

	void Clear();
};
