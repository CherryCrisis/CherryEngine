#pragma once

#include <array>
#include <map>
#include <memory>
#include <source_location>
#include <string>
#include <vector>

#include <cherry_macros.hpp>

#include "singleton.hpp"
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
	uint_least32_t line = 0u;

	bool		m_isSelected = false;
	FullDate	m_date;

	std::string file = "";
	std::string function = "";

	LogMessage*	m_logMessage;

public:
	Log(LogMessage* logMessage, FullDate fullDate, std::source_location sourceLocation)
		: m_logMessage(logMessage), m_date(fullDate),
		line(sourceLocation.line()), file(sourceLocation.file_name()), function(sourceLocation.file_name()) {}

	Log(LogMessage* logMessage, FullDate fullDate, unsigned int line, const char* file, const char* function)
		: m_logMessage(logMessage), m_date(fullDate),
		line(line), file(file), function(function) {}
};

class CCENGINE_API Debug : public Singleton<Debug>
{
private:
	//key size_t = hash of (string) message and (int) logType
	bool							m_isLogAdded = false;

	std::map<size_t, LogMessage>	m_logMessages;
	std::vector<Log>				m_logs;
	
	std::array<int, 3>				m_logTypeCounts{0,0,0};

	TimeManager* m_timeManager;

	inline void GetKeyOfLog(std::size_t& key, const std::string& message, const ELogType& logType);

public:
	Debug();

	std::vector<Log>* GetLogs() { return &m_logs; }
	const std::map<size_t, LogMessage>* GetCollapsedLogs() { return &m_logMessages; }
	const std::array<int, 3>* GetLogTypeCounts() { return &m_logTypeCounts; }
	
	// Print a string in the console and cache it
	void AddLog(ELogType logType, const char* message, std::source_location location =
		std::source_location::current());

	void AddLog(ELogType logType, const char* message, unsigned int line, const char* file, const char* function);

	void Clear();

	bool IsLogAdded() { return m_isLogAdded; }
	void ResetLogAddded() { m_isLogAdded = false; }
};

inline void Debug::GetKeyOfLog(std::size_t& key, const std::string& message, const ELogType& logType)
{
	/*https://stackoverflow.com/questions/19195183/how-to-properly-hash-the-custom-struct*/

	std::hash<std::string> hashStr;
	key ^= hashStr(message) + 0x9e3779b9 + (key << 6) + (key >> 2);

	std::hash<int> hashLogType;
	key ^= hashLogType(static_cast<int>(logType)) + 0x9e3779b9 + (key << 6) + (key >> 2);
}
