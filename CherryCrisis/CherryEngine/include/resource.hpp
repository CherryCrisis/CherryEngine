#pragma once

#include <string>
#include <memory>


class Resource
{
protected:
	const size_t hashId;
	const std::string filepath;

public:
	template <typename ResourceT>
	using Ref = std::shared_ptr<ResourceT>;

	Resource(const std::string& filepath)
		: hashId(std::hash<std::string>()(filepath)), filepath(filepath) {}

	virtual ~Resource() = default;

	const size_t GetHashId() const { return hashId; }
	const char* GetFilepath() const { return filepath.c_str(); }
};

