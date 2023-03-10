#include <pch.hpp>

#include <algorithm>
#include <cctype>
#include <string>

#include "debug.hpp"

#include "bool3.hpp"
#include "utils.hpp"


std::filesystem::path CopyTemporaryFile(const char* path)
{
	std::filesystem::path to(path);
	std::filesystem::path from = to;
	to.replace_extension("copy.dll");
	copy(from, to, std::filesystem::copy_options::update_existing);
	return to.generic_string();
}

namespace String
{
	std::string ExtractTypeIndexName(const std::type_index& typeindex)
	{
		return ExtractValue(typeindex.name(), ' ');
	}

	std::string ExtractValue(const std::string& str, const char key)
	{
		return str.substr(str.find_first_of(key) + 1);
	}

	std::string ExtractLastValue(const std::string& str, const char key)
	{
		return str.substr(str.find_last_of(key) + 1);
	}

	std::string ExtractValueStr(const std::string& str, const char* key)
	{
		std::string string = str;
		std::string token = str.substr(0, str.find(key));
		size_t pos = str.find(token);
		string.erase(pos, token.length());
		return string;
	}

	std::string ExtractKey(std::string& str, const char key, bool erase)
	{
		int len = static_cast<int>(str.find(key));

		std::string strr = str;

		if (len > 0)
		{
			if (erase)
			{
				str.erase(str.begin(), str.begin() + len + 1);
			}

			strr.erase(strr.begin() + len, strr.end());
			return strr;
		}

		return str;
	}

	std::string ExtractLastKey(std::string& str, const char key, bool erase)
	{
		int len = static_cast<int>(str.find_last_of(key));

		std::string strr = str;

		if (len > 0)
		{
			if (erase)
			{
				str.erase(str.begin(), str.begin() + len + 1);
			}

			strr.erase(strr.begin() + len, strr.end());
			return strr;
		}

		return str;
	}

	std::string ExtractKeyStr(const std::string& str, const char* key)
	{
		return str.substr(0, str.find(key));
	}

	void ReplaceAll(std::string& str, const std::string& from, const std::string& to)
	{
		if (from.empty())
			return;

		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos)
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}

	uint64_t ExtractUUID(const std::string& str)
	{
		std::string uuid = ExtractValue(str);

		uint64_t value;
		std::istringstream iss(uuid);
		iss >> value;
		return value;
	}

	float ExtractFloat(const std::string& str)
	{
		std::string vector = ExtractValue(str);
		return std::stof(vector);
	}

	int ExtractInt(const std::string& str)
	{
		std::string vector = ExtractValue(str);
		return std::stoi(vector);
	}

	CCMaths::Vector3 ExtractVector3(std::string str)
	{
		CCMaths::Vector3 value{};
		std::string temp = ExtractKey(str, '/', true);
		value.x = std::stof(temp);
		temp = ExtractKey(str, '/', true);
		value.y = std::stof(temp);
		temp = ExtractKey(str, '/', true);
		value.z = std::stof(temp);
		return value;
	}

	CCMaths::Vector2 ExtractVector2(std::string str)
	{
		CCMaths::Vector2 value{};
		std::string temp = ExtractKey(str, '/', true);
		value.x = std::stof(temp);
		temp = ExtractKey(str, '/', true);
		value.y = std::stof(temp);
		return value;
	}

	Bool3 ExtractBool3(std::string str)
	{
		Bool3 value{};
		std::string temp = ExtractKey(str, '/', true);
		value.x = std::stoi(temp);
		temp = ExtractKey(str, '/', true);
		value.y = std::stoi(temp);
		temp = ExtractKey(str, '/', true);
		value.z = std::stoi(temp);
		return value;
	}

	std::string ToLower(const std::string& str) 
	{
		std::string value = str;
		std::transform(value.begin(), value.end(), value.begin(),
			[](unsigned char c) { return std::tolower(c); });
		return value;
	}

	std::string ToUpper(const std::string& str) 
	{
		std::string value = str;
		std::transform(value.begin(), value.end(), value.begin(),
			[](unsigned char c) { return std::toupper(c); });
		return value;
	}
}

bool CreateFolder(const char* path, const char* name)
{
	std::filesystem::path newPath = path;
	
	if (!std::filesystem::exists(newPath)) 
	{
		Debug::GetInstance()->AddLog(ELogType::ERROR, "File location not valid");
		return false;
	}

	newPath /= name;
	if (std::filesystem::exists(newPath))
		std::filesystem::remove_all(newPath.c_str());

	std::filesystem::create_directory(newPath);
	
	return true;
}

bool CreateFolder(const std::string& path, const std::string& name)
{
	return CreateFolder(path.c_str(), name.c_str());
}

bool CopyFolder(const char* src, const char* dst)
{
	std::filesystem::copy(src, dst, std::filesystem::copy_options::recursive);
	return true;
}

bool CopyFile(const char* src, const char* dst)
{
	std::filesystem::copy(src, dst, std::filesystem::copy_options::overwrite_existing);
	return true;
}

bool CopyFile(const std::string& src, const std::string& dst)
{
	return CopyFile(src.c_str(), dst.c_str());
}


bool CopyFolder(const std::string& src, const std::string& dst)
{
	return CopyFolder(src.c_str(), dst.c_str());
}
