#pragma once

#include <string>
#include <vector>

#include <CherryMacros.h>

class CCENGINE_API LogMessage
{
public:
	std::string string;
	int count = 1;

	LogMessage(const char* value): string(std::string(value)) {}

	std::string CollapsedString() { std::string val = string; val += "    : " + std::to_string(count); return val; }
};


class CCENGINE_API Debug
{
private:
	static Debug* instance;

	std::vector<LogMessage> logs;

	Debug(Debug& other) = delete;
	void operator=(const Debug&) = delete;
	Debug();

public:
	static Debug* GetInstance();
	
	std::vector<LogMessage> GetLogs() { return logs; }
	
	// Print a string in the console and cache it
	void Log(const char* string);

	// Print all cached logs
	void PrintAllLogs();

	bool ContainsAndAdd(const char* string);

	void Clear() { logs.clear(); }
};
