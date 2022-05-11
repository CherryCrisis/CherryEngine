#include "pch.hpp"

#include <command.hpp>

#include <cstdio>
#include <iostream>

#include <windows.h>
#undef far
#undef near
#undef ERROR

#include <ShellAPI.h>
#include <tchar.h>

void call(const char* type, const char* command, const char* arg) 
{
	ShellExecuteA(NULL, type, command, arg, NULL, SW_SHOWDEFAULT);
}

void callEx(const char* type, const char* command, const char* arg)
{
	SHELLEXECUTEINFOA shellInfo = {0};
	shellInfo.nShow = SW_SHOWDEFAULT;
	shellInfo.lpVerb = type;
	shellInfo.lpFile = command;
	shellInfo.lpParameters = arg;
	shellInfo.fMask = SEE_MASK_FLAG_DDEWAIT | SEE_MASK_NO_CONSOLE | SEE_MASK_NOCLOSEPROCESS;
	shellInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
	if (ShellExecuteExA(&shellInfo)) 
	{
		std::cout << "Command line executed ..." << std::endl;
		WaitForSingleObject(shellInfo.hProcess, INFINITE);
	}
}