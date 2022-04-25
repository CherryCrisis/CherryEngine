#pragma once

#include <filesystem>
#include <string>
#include <maths/vector3.hpp>
#include <cherry_macros.hpp>

std::filesystem::path CopyTemporaryFile(const char* path);

struct Bool3;

namespace String
{
	//return the string chunk after the separator
	std::string CCENGINE_API ExtractValue(const std::string& str, const char separator = ':');
	
	//return the string chunk before the separator
	std::string CCENGINE_API ExtractKey(std::string& str, const char key = ':', bool erase = false);

	//return the string chunk before the substring
	std::string CCENGINE_API ExtractKeyStr(const std::string& str, const char* key = "");

	//return the string chunk after the substring
	std::string CCENGINE_API ExtractValueStr(const std::string& str, const char* key = "");

	//Replace "from" string to "to" string in the str
	void CCENGINE_API ReplaceAll(std::string& str, const std::string& from, const std::string& to);

	//-----------------------------------------------------
	// Extract the given value in the string and 
	// Casts it into the signature function
	//-----------------------------------------------------
	
	uint64_t		  CCENGINE_API ExtractUUID(const std::string& str);
	float			  CCENGINE_API ExtractFloat(const std::string& str);
	int			      CCENGINE_API ExtractInt(const std::string& str);
	CCMaths::Vector3  CCENGINE_API ExtractVector3(std::string str);
	Bool3			  CCENGINE_API ExtractBool3(std::string str);

	std::string CCENGINE_API ToLower(const std::string& str);
	std::string CCENGINE_API ToUpper(const std::string& str);
}

//CreateFolder
bool CCENGINE_API CreateFolder(const std::string& path, const std::string& name);
bool CCENGINE_API CreateFolder(const char* path, const char* name);

bool CCENGINE_API CopyFolder(const std::string& src, const std::string& dst);
bool CCENGINE_API CopyFolder(const char* src, const char* dst);

bool CCENGINE_API CopyFile(const std::string& src, const std::string& dst);
bool CCENGINE_API CopyFile(const char* src, const char* dst);