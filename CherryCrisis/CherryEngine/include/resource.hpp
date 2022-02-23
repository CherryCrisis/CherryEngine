#pragma once

#include <string>

class Resource
{
public:
	Resource(const std::string& filepath)
		: hashId(std::hash<std::string>()(filepath)), filepath(filepath) {}

	virtual ~Resource() = default;

	const size_t GetHashId() const { return hashId; }
	const std::string GetFilepath() const { return filepath; }

private:
	const size_t hashId;
	const std::string filepath;
};

