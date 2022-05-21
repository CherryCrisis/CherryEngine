#pragma once

#include <filesystem>
#include <string>


namespace String
{
	//return the string chunk after the separator
	std::string ExtractValue(const std::string& str, const char separator = ':');

	//return the string chunk before the separator
	std::string ExtractKey(std::string& str, const char key = ':', bool erase = false);

	//return the string chunk before the substring
	std::string ExtractKeyStr(const std::string& str, const char* key = "");

	//return the string chunk after the substring
	std::string ExtractValueStr(const std::string& str, const char* key = "");

	//Replace "from" string to "to" string in the str
	void ReplaceAll(std::string& str, const std::string& from, const std::string& to);

	//-----------------------------------------------------
	// Extract the given value in the string and 
	// Casts it into the signature function
	//-----------------------------------------------------

	uint64_t		   ExtractUUID(const std::string& str);
	float			   ExtractFloat(const std::string& str);
	int			       ExtractInt(const std::string& str);

}

//CreateFolder
bool CreateFolder(const std::string& path, const std::string& name);
bool CreateFolder(const char* path, const char* name);

bool CopyFolder(const std::string& src, const std::string& dst);
bool CopyFolder(const char* src, const char* dst);

bool CopyFile(const std::string& src, const std::string& dst);
bool CopyFile(const char* src, const char* dst);