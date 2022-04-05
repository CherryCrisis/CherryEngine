#pragma once

#include "singleton.hpp"

#include <string>
#include <map>
#include <vector>
#include <source_location>
#include <memory>

#include "cherry_macros.hpp"
#include "time_manager.hpp"

#undef ERROR

enum class CCENGINE_API ELogType : int
{
	INFO,
	WARNING,
	ERROR,
};

static const char* LogTypeCstr[] = { "INFO", "WARNING", "ERROR" };

struct LogMessage
{
	const std::string	m_logMessage	{};
	const ELogType		m_logType		{};
	unsigned int		m_count			{1};
};

class CCENGINE_API Log
{
public:
	LogMessage*	m_logMessage;
	FullDate	m_date;
	bool		m_isSelected = false;

	std::source_location m_sourceLocation;

public:
	Log(LogMessage* logMessage, FullDate fullDate, std::source_location sourceLocation)
		: m_logMessage(logMessage), m_date(fullDate), m_sourceLocation(sourceLocation) {}
};

class CCENGINE_API Debug : public Singleton<Debug>
{
private:

	//key size_t = hash of (string) message and (int) logType
	std::map<size_t, LogMessage>	m_logMessages;
	std::vector<Log>				m_logs;

	TimeManager* m_timeManager;

	inline void GetKeyOfLog(std::size_t& key, const std::string& message, const ELogType& logType);

public:
	Debug();

	std::vector<Log>* GetLogs() { return &m_logs; }
	std::map<size_t, LogMessage>* GetCollapsedLogs() { return &m_logMessages; }
	
	// Print a string in the console and cache it
	void AddLog(ELogType logType, const char* message, std::source_location location =
		std::source_location::current());

	void Clear();
};

inline void Debug::GetKeyOfLog(std::size_t& key, const std::string& message, const ELogType& logType)
{
	/*https://stackoverflow.com/questions/19195183/how-to-properly-hash-the-custom-struct*/

	std::hash<std::string> hashStr;
	key ^= hashStr(message) + 0x9e3779b9 + (key << 6) + (key >> 2);

	std::hash<int> hashLogType;
	key ^= hashLogType(static_cast<int>(logType)) + 0x9e3779b9 + (key << 6) + (key >> 2);
}
