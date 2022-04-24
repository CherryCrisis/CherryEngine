
#include <command.hpp>

#include <cstdio>
#include <iostream>

#include <windows.h>
#undef far
#undef near
#undef ERROR

#include <ShellAPI.h>

void call(const char* type, const char* command, const char* arg) 
{
	ShellExecuteA(NULL, type, command, arg, NULL, 10);
}