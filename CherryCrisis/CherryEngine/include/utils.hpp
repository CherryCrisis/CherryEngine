#pragma once

#include <filesystem>
#include <string>
#include <maths/vector3.hpp>
#include <cherry_macros.hpp>

std::filesystem::path CopyTemporaryFile(const char* path);

namespace String
{
	//return the string chunk after the separator
	std::string CCENGINE_API ExtractValue(const std::string& str, const char separator = ':');
	
	//return the string chunk before the separator
	std::string CCENGINE_API ExtractKey(std::string& str, const char key = ':', bool erase = false);

	//-----------------------------------------------------
	// Extract the given value in the string and 
	// Casts it into the signature function
	//-----------------------------------------------------
	
	uint64_t		  CCENGINE_API ExtractUUID(const std::string& str);
	float			  CCENGINE_API ExtractFloat(const std::string& str);
	int			      CCENGINE_API ExtractInt(const std::string& str);
	CCMaths::Vector3  CCENGINE_API ExtractVector3(std::string str);
}