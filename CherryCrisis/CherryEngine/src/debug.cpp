// debug.cpp : Defines the exported functions for the DLL.
#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier

#include "debug.hpp"

#include <iostream>

Debug* Debug::instance = nullptr;

void Debug::Log(const char* string)
{
	std::cout << string << std::endl;

	if (!ContainsAndAdd(string)) 
		logs.push_back(LogMessage(string));
}

void Debug::PrintAllLogs() 
{
	//for (const std::string& string : logs) 
	//{
	//	std::cout << string << std::endl;
	//}
}

bool Debug::ContainsAndAdd(const char* string) 
{
	for (LogMessage& log : logs)
	{
		const char* strings = log.string.c_str();
		if (!log.string.compare(string)) 
		{
			log.count++;
			return true;
		}
	}
	return false;
}

Debug::Debug()
{

}

Debug* Debug::GetInstance()
{
	if (instance == nullptr) {
		instance = new Debug();
	}
	return instance;
}