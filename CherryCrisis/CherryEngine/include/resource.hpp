#pragma once

#include <string>
#include <memory>

#include "event.hpp"

class Resource
{
protected:
	const size_t hashId;
	const std::string filepath;

public:
	template <typename ResourceT>
	using Ref = std::shared_ptr<ResourceT>;

	Event<std::shared_ptr<Resource>> m_onLoaded {};

	Resource(const std::string& filepath)
		: hashId(std::hash<std::string>()(filepath)), filepath(filepath) {}

	virtual ~Resource() = default;

	const size_t GetHashId() const { return hashId; }
	const char* GetFilepath() const { return filepath.c_str(); }

	void IsLoaded(std::shared_ptr<Resource> resource) { m_onLoaded.Invoke(resource); }
};

