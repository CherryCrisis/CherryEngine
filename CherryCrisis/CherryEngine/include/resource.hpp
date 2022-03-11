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

	std::shared_ptr<Event<Ref<Resource>>> m_onLoaded;

	Resource(const std::string& filepath)
		: hashId(std::hash<std::string>()(filepath)), filepath(filepath) {}

	virtual ~Resource() = default;

	const size_t GetHashId() const { return hashId; }
	const char* GetFilepath() const { return filepath.c_str(); }
};

//class ResourceMultithread : public Resource
//{
//protected:
//	Event<ResourceMultithread*>* m_onLoaded;
//
//public :
//	ResourceMultithread(const std::string& filepath)
//		: Resource(filepath) {}
//
//	void ResourceLoaded() { m_onLoaded->Invoke(this); }
//};

