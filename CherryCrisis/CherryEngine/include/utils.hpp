#pragma once

#include <filesystem>
#include <string>
#include <maths/vector3.hpp>
#include <cherry_macros.hpp>
#include <typeindex>

std::filesystem::path CopyTemporaryFile(const char* path);

struct Bool3;

namespace String
{
	// return the raw type name of a typeindex
	CCENGINE_API std::string ExtractTypeIndexName(const std::type_index& typeindex);

	// return the raw type name of a typeindex
	template <typename T>
	std::string ExtractTypeName()
	{
		return ExtractTypeIndexName(typeid(T));
	}

	//return the string chunk after the separator
	CCENGINE_API std::string ExtractValue(const std::string& str, const char separator = ':');

	//return the string chunk after the separator
	CCENGINE_API std::string ExtractLastValue(const std::string& str, const char separator = ':');
	
	//return the string chunk before the separator
	CCENGINE_API std::string ExtractKey(std::string& str, const char key = ':', bool erase = false);

	//return the string chunk before the separator
	CCENGINE_API std::string ExtractLastKey(std::string& str, const char key = ':', bool erase = false);

	//return the string chunk before the substring
	CCENGINE_API std::string ExtractKeyStr(const std::string& str, const char* key = "");

	//return the string chunk after the substring
	CCENGINE_API std::string ExtractValueStr(const std::string& str, const char* key = "");

	//Replace "from" string to "to" string in the str
	CCENGINE_API void ReplaceAll(std::string& str, const std::string& from, const std::string& to);

	//-----------------------------------------------------
	// Extract the given value in the string and 
	// Casts it into the signature function
	//-----------------------------------------------------
	
	uint64_t		  CCENGINE_API ExtractUUID(const std::string& str);
	float			  CCENGINE_API ExtractFloat(const std::string& str);
	int			      CCENGINE_API ExtractInt(const std::string& str);
	CCMaths::Vector3  CCENGINE_API ExtractVector3(std::string str);
	CCMaths::Vector2  CCENGINE_API ExtractVector2(std::string str);
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
