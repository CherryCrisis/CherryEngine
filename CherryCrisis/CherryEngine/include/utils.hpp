#pragma once

#include <filesystem>
#include <string>
#include <maths/vector3.hpp>

std::filesystem::path CopyTemporaryFile(const char* path);

namespace String
{
	//return the string chunk after the separator
	std::string ExtractValue(const std::string& str, const char separator = ':');
	
	//return the string chunk before the separator
	std::string ExtractKey(std::string& str, const char key = ':', bool erase = false);

	//-----------------------------------------------------
	// Extract the given value in the string and 
	// Casts it into the signature function
	//-----------------------------------------------------
	
	uint64_t		  ExtractUUID(const std::string& str);
	float			  ExtractFloat(const std::string& str);
	int			      ExtractInt(const std::string& str);
	CCMaths::Vector3  ExtractVector3(std::string str);
}