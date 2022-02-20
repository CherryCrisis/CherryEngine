// debug.cpp : Defines the exported functions for the DLL.
#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier

#include "debug.hpp"

#include <iostream>

Debug* Debug::instance = nullptr;

void Debug::Log(const char* string)
{
	std::cout << string << std::endl;
	logs.push_back(std::string(string));
}

void Debug::PrintAllLogs() 
{
	for (const std::string& string : logs) 
	{
		std::cout << string << std::endl;
	}
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