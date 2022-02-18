#pragma once

#include <string>
#include <vector>

#include <CherryMacros.h>

class CCENGINE_API Debug
{
private:
	static Debug* instance;

	std::vector<std::string> logs;

	Debug(Debug& other) = delete;
	void operator=(const Debug&) = delete;
	Debug();
public:
	static Debug* GetInstance();
	
	std::vector<std::string> GetLogs() { return logs; }
	
	// Print a string in the console and cache it
	void Log(const char* string);

	// Print all cached logs
	void PrintAllLogs();

	void Clear() { logs.clear(); }
};